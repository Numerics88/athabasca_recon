/*
Copyright (C) 2011-2015 Eric Nodwell
enodwell@ucalgary.ca

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ReconstructionMessages.hpp"
#include "io/AsynchronousIOProcessor.hpp"
#include "projection_correction/AttenuationCalculator.hpp"
#ifdef USE_VDSP
#include "filtering/ProjectionFilterer_vDSP.hpp"
#endif
#ifdef USE_FFTW
#include "filtering/ProjectionFilterer_fftw.hpp"
#endif
#include "filtering/ProjectionFilterer_Convolution.hpp"
#include "back_projection/ParallelBackProjector_cpu.hpp"
#include "back_projection/PixelInterpolator.hpp"
#include "back_projection/RayTracer.hpp"
#include "util/Subvolumer.hpp"
#include "util/LinearFit.hpp"
#include "AthabascaException.hpp"
#include "bonelab/DoubleBuffer.hpp"
#include "bonelab/SynchronizeMessage.hpp"
#include "bonelab/Image.hpp"
#include <boost/math/constants/constants.hpp>

using boost::format;
namespace constants = boost::math::constants;

namespace athabasca_recon
  {

  template <class TProjectionIn, class TProjectionOut, class TVolume>
  MultiThreadedReconstructionRunner<TProjectionIn, TProjectionOut, TVolume>
  ::MultiThreadedReconstructionRunner()
    :
    m_NumberOfThreads(boost::thread::hardware_concurrency()),
    m_BarrierMeAndAllWorkers(NULL),
    m_BarrierAllWorkers(NULL),
    m_BarrierMeAndOneWorker(NULL)
    {}

  template <class TProjectionIn, class TProjectionOut, class TVolume>
  MultiThreadedReconstructionRunner<TProjectionIn, TProjectionOut, TVolume>
  ::~MultiThreadedReconstructionRunner()
    {
    delete m_BarrierMeAndAllWorkers;
    delete m_BarrierAllWorkers;
    delete m_BarrierMeAndOneWorker;
    }

  template <class TProjectionIn, class TProjectionOut, class TVolume>
  void MultiThreadedReconstructionRunner<TProjectionIn, TProjectionOut, TVolume>
  ::Execute()
    {
    // Require exactly one type of projections reader.
    athabasca_assert(((this->m_RawProjectionsReader != NULL) + 
                      (this->m_AttenuationProjectionsReader != NULL) + 
                      (this->m_FilteredProjectionsReader != NULL)) == 1);
    athabasca_assert(this->m_NumberOfThreads > 0);

    std::cout << "Launching " << this->m_NumberOfThreads << " worker threads.\n";
    
    // Create threads
    bonelab::ObjectGroup<bonelab::ThreadRunner> threadRunners(this->m_NumberOfThreads);
    boost::thread_group threads;
    for (int t=0; t < this->m_NumberOfThreads; ++t)
      {
      threads.create_thread(boost::bind(&bonelab::ThreadRunner::Run, &threadRunners[t]));
      threadRunners[t].SetMessage(new CreateThreadWorkerMessage<TProjectionOut,TVolume>());
      }
    // Create some barrier objects for synchronization.
    // Note that these must stay in existence until after join_all.
    this->m_BarrierMeAndAllWorkers = new boost::barrier(this->m_NumberOfThreads+1);
    this->m_BarrierAllWorkers = new boost::barrier (this->m_NumberOfThreads);
    this->m_BarrierMeAndOneWorker = new boost::barrier(2);

    try
      {
      ExecuteOnThreads(threadRunners);
      }
    catch (...)
      {
      std::cerr << "\nException occured.  Waiting for threads to exit...\n";
      for (int t=0; t<this->m_NumberOfThreads; ++t)
        { threadRunners[t].Stop(); }
      threads.join_all();
      throw;
      }

    // Stopping the ThreadRunner will cause the destructors of its objects to be called.
    for (int t=0; t<this->m_NumberOfThreads; ++t)
      { threadRunners[t].Stop(); }
    threads.join_all();
    }


  template <class TProjectionIn, class TProjectionOut, class TVolume>
  void MultiThreadedReconstructionRunner<TProjectionIn, TProjectionOut, TVolume>
  ::ExecuteOnThreads(bonelab::ObjectGroup<bonelab::ThreadRunner>& threadRunners)
    {
    // Create data objects.  Not all of these will necessarily be used.
    bonelab::ObjectGroup<bonelab::DoubleBuffer<TProjectionIn> > raw_projections;
    bonelab::ObjectGroup<bonelab::DoubleBuffer<TProjectionOut> > attenuation_projections;
    bonelab::ObjectGroup<bonelab::DoubleBuffer<TProjectionOut> > filtered_projections;

    int numberOfProjections = 0;
    if (this->m_RawProjectionsReader)
      {
      raw_projections.template construct<bonelab::DoubleBuffer<TProjectionIn> >(this->m_NumberOfThreads);
      attenuation_projections.template construct<bonelab::DoubleBuffer<TProjectionOut> >(this->m_NumberOfThreads);
      filtered_projections.template construct<bonelab::DoubleBuffer<TProjectionOut> >(this->m_NumberOfThreads);
      for (int p=0; p<this->m_NumberOfThreads; ++p)
        {
        raw_projections[p].construct(
            this->m_RawProjectionsReader->GetDimensions(),
            this->m_RawProjectionsReader->GetPixelSize(),
            this->m_RawProjectionsReader->GetOrigin());
        attenuation_projections[p].construct(
            this->m_RawProjectionsReader->GetDimensions(),
            this->m_RawProjectionsReader->GetPixelSize(),
            this->m_RawProjectionsReader->GetOrigin());
        if (this->m_FilteredProjectionsWriter || this->m_VolumeWriter)
          {
          filtered_projections[p].construct(
              this->m_RawProjectionsReader->GetDimensions(),
              this->m_RawProjectionsReader->GetPixelSize(),
              this->m_RawProjectionsReader->GetOrigin());
          }
        }
      numberOfProjections = this->m_RawProjectionsReader->GetNumberOfProjections();
      }
    else if (this->m_AttenuationProjectionsReader)
      {
      attenuation_projections.template construct<bonelab::DoubleBuffer<TProjectionOut> >(this->m_NumberOfThreads);
      filtered_projections.template construct<bonelab::DoubleBuffer<TProjectionOut> >(this->m_NumberOfThreads);
      for (int p=0; p<this->m_NumberOfThreads; ++p)
        {
        attenuation_projections[p].construct(
            this->m_AttenuationProjectionsReader->GetDimensions(),
            this->m_AttenuationProjectionsReader->GetPixelSize(),
            this->m_AttenuationProjectionsReader->GetOrigin());
        filtered_projections[p].construct(
            this->m_AttenuationProjectionsReader->GetDimensions(),
            this->m_AttenuationProjectionsReader->GetPixelSize(),
            this->m_AttenuationProjectionsReader->GetOrigin());
        }
      numberOfProjections = this->m_AttenuationProjectionsReader->GetNumberOfProjections();
      }
    else if (this->m_FilteredProjectionsReader)
      {
      filtered_projections.template construct<bonelab::DoubleBuffer<TProjectionOut> >(this->m_NumberOfThreads);
      for (int p=0; p<this->m_NumberOfThreads; ++p)
        {
        filtered_projections[p].construct(
            this->m_FilteredProjectionsReader->GetDimensions(),
            this->m_FilteredProjectionsReader->GetPixelSize(),
            this->m_FilteredProjectionsReader->GetOrigin());
        }
      numberOfProjections = this->m_FilteredProjectionsReader->GetNumberOfProjections();
      }

    // Determine angle increment
    TSpace deltaTheta;
    if (this->m_ProjectionsAtBothLimits)
      { deltaTheta = constants::pi<TSpace>()/(numberOfProjections-1); }
    else
      { deltaTheta = constants::pi<TSpace>()/numberOfProjections; }
    if (this->m_ReverseRotation)
      { deltaTheta = -deltaTheta; }
    
    // Create IO serializer.
    AsynchronousIOProcessor<TProjectionIn,TProjectionOut> asynchronousIOProcessor;

    // Create processing objects.  Not all of these will necessarily be used.
    AttenuationCalculator<TProjectionIn,TProjectionOut> attenuationCalculator;
    
    if (this->m_RawProjectionsReader)
      {
      attenuationCalculator.SetCalibration(this->m_Calibration);
      }

    if ((!this->m_FilteredProjectionsReader) &&
        (this->m_FilteredProjectionsWriter || this->m_VolumeWriter))
      {
      if (this->m_FilteringModule == FILTERING_MODULE_CONVOLUTION)
        {
        for (int t=0; t < this->m_NumberOfThreads; ++t)
          {
          threadRunners[t].SetMessage(new CreateProjectionFiltererMessage<TProjectionOut,TVolume,
              ProjectionFilterer_Convolution<TProjectionOut> >(
                  filtered_projections[0].dims(),
                  filtered_projections[0].spacing()[1],
                  fabs(deltaTheta),
                  this->m_SmoothingFilter));
          }
        }
#ifdef USE_VDSP
      else if (this->m_FilteringModule == FILTERING_MODULE_VDSP)
        {
        for (int t=0; t < this->m_NumberOfThreads; ++t)
          {
          threadRunners[t].SetMessage(new CreateProjectionFiltererMessage<TProjectionOut,TVolume,
              ProjectionFilterer_vDSP<TProjectionOut> >(
                  filtered_projections[0].dims(),
                  filtered_projections[0].spacing()[1],
                  fabs(deltaTheta),
                  this->m_SmoothingFilter));
          }
        }
#endif
#ifdef USE_FFTW
      else if (this->m_FilteringModule == FILTERING_MODULE_FFTW)
        {
        for (int t=0; t < this->m_NumberOfThreads; ++t)
          {
          threadRunners[t].SetMessage(new CreateProjectionFiltererMessage<TProjectionOut,TVolume,
              ProjectionFilterer_fftw<TProjectionOut> >(
                  filtered_projections[0].dims(),
                  filtered_projections[0].spacing()[1],
                  fabs(deltaTheta),
                  this->m_SmoothingFilter));
          }
        }
#endif
      else
        { throw_athabasca_exception("Internal error\n"); }
      }

    if (this->m_VolumeWriter)
      {
      // Create BackProjectors in each thread
      if (this->m_PixelInterpolation == PIXEL_INTERPOLATION_NEAREST)
        {
        for (int t=0; t<this->m_NumberOfThreads; ++t)
          {
          threadRunners[t].SetMessage(new CreateBackProjectorMessage<TProjectionOut,TVolume,
              NearestNeighborPixelInterpolator<TProjectionOut>,
              ParallelRayTracer<TSpace> >(this->m_ScalingFactor));
          }
        }
      else if (this->m_PixelInterpolation == PIXEL_INTERPOLATION_BILINEAR)
        {
        for (int t=0; t<this->m_NumberOfThreads; ++t)
          {
          threadRunners[t].SetMessage(new CreateBackProjectorMessage<TProjectionOut,TVolume,
              BilinearPixelInterpolator<TProjectionOut>,
              ParallelRayTracer<TSpace> >(this->m_ScalingFactor));
          }
        }
      else if (this->m_PixelInterpolation == PIXEL_INTERPOLATION_BILINEAR_WITH_FALLBACK)
        {
        for (int t=0; t<this->m_NumberOfThreads; ++t)
          {
          threadRunners[t].SetMessage(new CreateBackProjectorMessage<TProjectionOut,TVolume,
              BilinearPixelInterpolatorWithFallback<TProjectionOut>,
              ParallelRayTracer<TSpace> >(this->m_ScalingFactor));
          }
        }
      else
        { throw_athabasca_exception("Internal Error."); }
      // Might eventually be needed outside the scope of this conditional, but
      // for now it is only needed here and can live here.
      Subvolumer<TVolume> subvolumer(
          this->m_VolumeDims,
          this->m_VoxelSize,
          this->m_VolumeOrigin,
          this->m_NumberOfThreads);
      for (int t=0; t<this->m_NumberOfThreads; ++t)
        {
        threadRunners[t].SetMessage(
              new AllocateVolumeMessage<TProjectionOut,TVolume>(
                  subvolumer.GetSubvolumeDims(t),
                  this->m_VoxelSize,
                  subvolumer.GetSubvolumeOrigin(t)));
        }
      }

    // If we don't actually need to process any projections, take
    // them singly.
    int projectionsPerStep = this->m_FilteredProjectionsReader ? 1 : this->m_NumberOfThreads;
    projectionsPerStep = std::min(projectionsPerStep, numberOfProjections);

    // With double-buffering need to issue requests to load the first projections.
    if (this->m_RawProjectionsReader)
      {
      for (int pp=0; pp<projectionsPerStep; ++pp)
        {
        asynchronousIOProcessor.RequestIOOperation(this->m_RawProjectionsReader, &raw_projections[pp].loading());
        }
      }
    if (this->m_AttenuationProjectionsReader)
      {
      for (int pp=0; pp<projectionsPerStep; ++pp)
        {
        asynchronousIOProcessor.RequestIOOperation(this->m_AttenuationProjectionsReader, &attenuation_projections[pp].loading());
        }
      }
    if (this->m_FilteredProjectionsReader)
      {
      for (int pp=0; pp<projectionsPerStep; ++pp)
        {
        asynchronousIOProcessor.RequestIOOperation(this->m_FilteredProjectionsReader, &filtered_projections[pp].loading());
        }
      }

    // Allocating some storage that we will need in the loop.
    std::vector<TProjectionOutValue> angles;
    angles.resize(projectionsPerStep);
    bonelab::Array<1,double,TIndex> corrections(numberOfProjections);

    // Execute by looping over projection groups.

    this->Print("\n");
    for (int p=0; p < numberOfProjections; p += projectionsPerStep)
      {
      // Don't try to process more projections than there are.
      int projectionsThisStep = std::min(projectionsPerStep, numberOfProjections-p);
      int projectionsNextStep = std::min(projectionsPerStep, numberOfProjections-p-projectionsPerStep);
      
      for (int pp=0; pp<projectionsThisStep; ++pp)
        { angles[pp] = (p+pp)*deltaTheta;  }
      this->Print(format("Projections %d-%d at angles %.2f to %.2f : ")
                    % p % (p+projectionsThisStep-1)
                    % (angles[0]*180/constants::pi<TProjectionOutValue>())
                    % (angles[projectionsThisStep-1]*180/constants::pi<TProjectionOutValue>()));

      // If writing attenuation output, swap buffers before using.
      if (this->m_AttenuationProjectionsWriter)
        {
        for (int pp=0; pp<projectionsPerStep; ++pp)
          { attenuation_projections[pp].swap(); }
        // Note that not until the 3rd pass will we be attempting to re-use a buffer.
        if (p > projectionsPerStep)
          {
          for (int pp=0; pp<projectionsThisStep; ++pp)
            {
            threadRunners[pp].SetMessage(
                new WaitForBufferReleaseMessage<TProjectionIn,TProjectionOut>(
                      asynchronousIOProcessor,
                      &attenuation_projections[pp].active()));
            }
          }
        }

      if (this->m_RawProjectionsReader)
        {
        this->Print("R ");
        for (int pp=0; pp<projectionsPerStep; ++pp)
          { raw_projections[pp].swap(); }
        for (int pp=0; pp<projectionsNextStep; ++pp)
          {
          asynchronousIOProcessor.RequestIOOperation(
                    this->m_RawProjectionsReader,
                    &raw_projections[pp].loading());
          }
        for (int pp=0; pp<projectionsThisStep; ++pp)
          {
          threadRunners[pp].SetMessage(
              new WaitForBufferReleaseMessage<TProjectionIn,TProjectionOut>(
                    asynchronousIOProcessor,
                    &raw_projections[pp].active()));
          }

        this->Print("P ");
        for (int pp=0; pp<projectionsThisStep; ++pp)
          {
          threadRunners[pp].SetMessage(
              new ConvertToAttenuationMessage<TProjectionIn, TProjectionOut>(
                  attenuationCalculator,
                  raw_projections[pp].active(),
                  attenuation_projections[pp].active()));
          }
        if (this->m_BadPixelCorrector)
          {
          this->Print("C ");
          for (int pp=0; pp<projectionsThisStep; ++pp)
            {
            threadRunners[pp].SetMessage(
              new CorrectBadPixelsMessage<TProjectionOut>(
                     *this->m_BadPixelCorrector, attenuation_projections[pp].active()));
            }
          }
        if (this->m_BeamPowerCorrector)
          {
          this->Print("X ");
          if (p == 0)
            {
            bonelab::SynchronizeAllAndWait(threadRunners, *this->m_BarrierMeAndAllWorkers);            
            this->m_BeamPowerCorrector->SetReferenceProjection(attenuation_projections[0].active());
            }
          for (int pp=0; pp<projectionsThisStep; ++pp)
            {
            threadRunners[pp].SetMessage(
              new CorrectBeamPowerMessage<TProjectionOut>(
                     *this->m_BeamPowerCorrector,
                     p+pp,
                     attenuation_projections[pp].active(),
                     &corrections[p+pp]));
            }
          }
        }

      if (this->m_AttenuationProjectionsReader)
        {
        this->Print("R ");
        for (int pp=0; pp<projectionsPerStep; ++pp)
          { attenuation_projections[pp].swap(); }
        for (int pp=0; pp<projectionsNextStep; ++pp)
          {
          asynchronousIOProcessor.RequestIOOperation(
                    this->m_AttenuationProjectionsReader,
                    &attenuation_projections[pp].loading());
          }
        for (int pp=0; pp<projectionsThisStep; ++pp)
          {
          threadRunners[pp].SetMessage(
            new WaitForBufferReleaseMessage<TProjectionIn,TProjectionOut>(
                    asynchronousIOProcessor,
                    &attenuation_projections[pp].active()));
          }
        }

      // If writing filtered output, swap buffers before using.
      if (this->m_FilteredProjectionsWriter)
        {
        for (int pp=0; pp<projectionsPerStep; ++pp)
          { filtered_projections[pp].swap(); }
        // Note that not until the 3rd pass will we be attempting to re-use a buffer.
        if (p > projectionsPerStep)
          {
          for (int pp=0; pp<projectionsThisStep; ++pp)
            {
            threadRunners[pp].SetMessage(
                new WaitForBufferReleaseMessage<TProjectionIn,TProjectionOut>(
                      asynchronousIOProcessor,
                      &filtered_projections[pp].active()));
            }
          }
        }

      if (this->m_FilteredProjectionsReader)
        {
        this->Print("R ");
        for (int pp=0; pp<projectionsPerStep; ++pp)
          { filtered_projections[pp].swap(); }
        for (int pp=0; pp<projectionsNextStep; ++pp)
          {
          asynchronousIOProcessor.RequestIOOperation(
                    this->m_FilteredProjectionsReader,
                    &filtered_projections[pp].loading());
          }
        for (int pp=0; pp<projectionsThisStep; ++pp)
          {
          threadRunners[pp].SetMessage(
            new WaitForBufferReleaseMessage<TProjectionIn,TProjectionOut>(
                    asynchronousIOProcessor,
                    &filtered_projections[pp].active()));
          }
        }
      else if (this->m_FilteredProjectionsWriter || this->m_VolumeWriter)
        {      
        this->Print("F ");
        for (int pp=0; pp<projectionsThisStep; ++pp)
          {
          threadRunners[pp].SetMessage(
              new FilterProjectionMessage<TProjectionOut,TVolume>(
                     attenuation_projections[pp].active(),
                     filtered_projections[pp].active()));
          }
        }

      // Can't proceed to backprojection until all current filtered projections are ready.
      bonelab::SynchronizeAllAndWait(threadRunners, *this->m_BarrierMeAndAllWorkers);
      // At this point, check and re-throw exceptions in worker threads
      for (int pp=0; pp<projectionsThisStep; ++pp)
        { threadRunners[pp].CheckException(); }

      // Now that we have all the projection stuff calculated, we can write out
      // things to output files.
      if (this->m_AttenuationCorrectionsFile)
        {
        for (int pp=0; pp<projectionsThisStep; ++pp)
          { *this->m_AttenuationCorrectionsFile << corrections[p+pp] << '\n'; }
        }
      if (this->m_AttenuationProjectionsWriter)
        {
        this->Print("W ");
        for (int pp=0; pp<projectionsThisStep; ++pp)
          {
          asynchronousIOProcessor.RequestIOOperation(
                    this->m_AttenuationProjectionsWriter,
                    &attenuation_projections[pp].active());
          }
        }
      if (this->m_FilteredProjectionsWriter)
        {
        this->Print("W ");
        for (int pp=0; pp<projectionsThisStep; ++pp)
          {
          asynchronousIOProcessor.RequestIOOperation(
                    this->m_FilteredProjectionsWriter,
                    &filtered_projections[pp].active());
          }
        }

      // Back-project if we are generating a volume.
      // Note that every projection has to be processed for each subvolume.
      if (this->m_VolumeWriter)
        {
        if (!(this->m_ProjectionsAtBothLimits && p == numberOfProjections-1))
          { this->Print("B "); }
        for (int pp=0; pp<projectionsThisStep; ++pp)
          {
          if (!(this->m_ProjectionsAtBothLimits && p+pp == numberOfProjections-1))
            {
            for (int t=0; t<this->m_NumberOfThreads; ++t)
              {
              threadRunners[t].SetMessage(
                  new ApplyProjectionMessage<TProjectionOut,TVolume>(
                          filtered_projections[pp].active(), angles[pp]));
              }
            }
          }
        // Wait for all threads to complete (will otherwise be clobbering old data on next loop iteration)
        bonelab::SynchronizeAllAndWait(threadRunners, *this->m_BarrierMeAndAllWorkers);
        // At this point, check and re-throw exceptions in worker threads.
        for (int pp=0; pp<projectionsThisStep; ++pp)
          { threadRunners[pp].CheckException(); }
        }

      this->Print("\n");
      }  // end of execution loop of projection groups

    asynchronousIOProcessor.WaitForCompletion();

    if (this->m_VolumeWriter)
      {
      std::cout << "Writing out volume data.\n";
      for (int t=0; t<this->m_NumberOfThreads; ++t)
        {
        threadRunners[t].SetMessage(
              new WriteVolumeDataMessage<TProjectionOut,TVolume>(
                      this->m_VolumeWriter));
        bonelab::SynchronizeOneAndWait(threadRunners[t], *this->m_BarrierMeAndOneWorker);
        }
      }

    if (this->m_AttenuationCorrectionsFile)
      {
      bonelab::Array<1,double,TIndex> x(numberOfProjections);
      for (TIndex i=0; i<numberOfProjections; ++i) {x[i] = i;}
      LinearFit<bonelab::Array<1,double,TIndex> >fit(x,corrections);
      std::cout << "Linear fit to beam power corrections gives "
                << fit.GetConstantTerm() << ", "
                << fit.GetLinearTerm() << " .\n";
      }

    // Wait for all threads to complete
    bonelab::SynchronizeAllAndWait(threadRunners, *this->m_BarrierMeAndAllWorkers);
    }

  } // namespace athabasca_recon
