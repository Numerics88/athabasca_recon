/*
Copyright (C) 2011 Eric Nodwell
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

import java.awt.*;
import java.awt.event.*;
import javax.swing.BoxLayout;
import java.io.*;
import ij.plugin.frame.*;
import ij.*;
import ij.process.*;
import ij.gui.*;
import ij.io.*;
import ij.plugin.FileInfoVirtualStack;
import ij.measure.Calibration;
import java.lang.Math;
import org.apache.commons.math.analysis.DifferentiableMultivariateRealFunction;
import org.apache.commons.math.analysis.MultivariateRealFunction;
import org.apache.commons.math.analysis.MultivariateVectorialFunction;
import org.apache.commons.math.optimization.MultivariateRealOptimizer;
import org.apache.commons.math.optimization.direct.PowellOptimizer;
import org.apache.commons.math.optimization.GoalType;
import org.apache.commons.math.optimization.RealPointValuePair;
import org.apache.commons.math.optimization.RealConvergenceChecker;
import org.apache.commons.math.util.MathUtils;
import org.apache.commons.math.util.FastMath;
import net.sf.ij_plugins.io.metaimage.MiEncoder;

public class Align_Projections extends PlugInFrame implements ActionListener,
Runnable,DifferentiableMultivariateRealFunction {

	Panel panel;
	static Frame instance;
	private Button updateButton;
	private Button optimizeButton;
	private Button applyButton;
	private Button resetButton;
	private ImagePlus       sourceStackImp;
  private TextField centerPixelText;
  private TextField detectorAngleText;
  private TextField horizontalBorderText;
  private TextField topBorderText;
  private TextField bottomBorderText;
  private Label crossCorrelationText;
  private double centerPixel;
  private double detectorAngle;
  private int horizontalBorder;
  private int topBorder;
  private int bottomBorder;
  private double crossCorrelation;
  private ImageProcessor unalteredFirstImage;
  private ImageProcessor unalteredLastImage;
  private ImageProcessor firstImage;
  private ImageProcessor lastImage;
  private ImagePlus impFirstImage;
  private ImagePlus impLastImage;
  private ImagePlus impOverlayImage;
  private boolean stopTuning;
  private double tuningWeights[];
	private Thread thread;

  public Align_Projections() {
		super("Align Projections");
	}

	public void run(String arg) {
		if (instance!=null) {
			instance.toFront();
			return;
		}
		instance = this;
		addKeyListener(IJ.getInstance());

	  if (sourceStackImp == null) {
		  sourceStackImp = WindowManager.getCurrentImage();
		  if (sourceStackImp != null) {
		    centerPixel = 0.5*(sourceStackImp.getWidth()-1);
		  } else {
		    centerPixel = 50;  // completely arbitrary
	    }
	  }

    setLayout(new BoxLayout(this, BoxLayout.PAGE_AXIS));

    Panel valuesPanel = new Panel();
		valuesPanel.setLayout(new GridLayout(6, 2));

    Label centerPixelLabel = new Label("Center Pixel");
    valuesPanel.add(centerPixelLabel);
    centerPixelText = new TextField(IJ.d2s(centerPixel, 1), 15);
    valuesPanel.add(centerPixelText);

    Label detectorAngleLabel = new Label("Detector Angle");
    valuesPanel.add(detectorAngleLabel);
    detectorAngleText = new TextField(IJ.d2s(0, 4), 15);
    valuesPanel.add(detectorAngleText);

    Label horizontalBorderLabel = new Label("Horizontal Border");
    valuesPanel.add(horizontalBorderLabel);
    horizontalBorderText = new TextField("0", 15);
    valuesPanel.add(horizontalBorderText);

    Label topBorderLabel = new Label("Top Border");
    valuesPanel.add(topBorderLabel);
    topBorderText = new TextField("0", 15);
    valuesPanel.add(topBorderText);
    
    Label bottomBorderLabel = new Label("Bottom Border");
    valuesPanel.add(bottomBorderLabel);
    bottomBorderText = new TextField("0", 15);
    valuesPanel.add(bottomBorderText);
    
    Label crossCorrelationLabel = new Label("Cross-Correlation");
    valuesPanel.add(crossCorrelationLabel);
    crossCorrelationText = new Label("-");
    valuesPanel.add(crossCorrelationText);

    this.add(valuesPanel);

		updateButton = addButton("Update");
		optimizeButton = addButton("Optimize");
		applyButton = addButton("Apply to stack and save");
		resetButton = addButton("Reset");
				
		pack();
		GUI.center(this);
		show();
	}

	Button addButton(String label) {
		Button b = new Button(label);
		b.addActionListener(this);
		b.addKeyListener(IJ.getInstance());
		add(b);
		return b;
	}

	public void updateAction() {	  
	  if (sourceStackImp == null) {
		  sourceStackImp = WindowManager.getCurrentImage();
  	  if (sourceStackImp == null) {
  	    IJ.error("No stack selected");
  	    return;
	    }
      if (sourceStackImp.getStack() == null) {
  	    IJ.error("No stack selected");
  	    sourceStackImp = null;
  	    return;
	    }
      if (sourceStackImp.getStackSize() < 2) {
  	    IJ.error("Stack must be of size >= 2");
  	    sourceStackImp = null;
  	    return;
	    }
	  }
	  detectorAngle = Double.valueOf(detectorAngleText.getText()).doubleValue();
	  centerPixel = Double.valueOf(centerPixelText.getText()).doubleValue();
	  horizontalBorder = Integer.valueOf(horizontalBorderText.getText()).intValue();
	  topBorder = Integer.valueOf(topBorderText.getText()).intValue();
	  bottomBorder = Integer.valueOf(bottomBorderText.getText()).intValue();

    getFirstAndLast();
    fiddleImages();  
    showFirstAndLast();
    computeCrossCorrelation();
    updateCrossCorrelation();
    showOverlay();
	}

  void updateCrossCorrelation() {
    crossCorrelationText.setText(String.format("%1$,.8f",crossCorrelation));
  }

  // Used in tuning as function to be optimized.
  public double value(double[] x) {
    // IJ.log("Evaluating");
    centerPixel = x[0]/tuningWeights[0];
    detectorAngle = x[1]/tuningWeights[1];
    fiddleImages();  
    computeCrossCorrelation();
    // To exit the optimizer quickly, we throw an exception; this is
    // caught in run().
    // if (stopTuning) {
    //   // IJ.log("Throwing GetMeOuttaHereException");
    //   throw new GetMeOuttaHereException();
    // }
    UpdateOverlayAndControls();
    return crossCorrelation;
  }

  public void UpdateOverlayAndControls() {
    centerPixelText.setText(IJ.d2s(centerPixel, 6));
    detectorAngleText.setText(IJ.d2s(detectorAngle, 6));
    updateCrossCorrelation();
    showFirstAndLast();
    showOverlay();
  }

  void getFirstAndLast() {
    int width = sourceStackImp.getWidth();
    int height = sourceStackImp.getHeight();
    int stackSize = sourceStackImp.getStackSize();
    ImageStack stack = sourceStackImp.getStack();
    // ImageStack stack2 = new ImageStack(width, height, sourceStackImp.getProcessor().getColorModel());

    int currSlice = 1;
    unalteredFirstImage = stack.getProcessor(currSlice);
    unalteredFirstImage = unalteredFirstImage.crop();  // Creates a new processor.
    unalteredFirstImage = unalteredFirstImage.convertToFloat();

    currSlice = stackSize;
    unalteredLastImage = stack.getProcessor(currSlice);
    unalteredLastImage = unalteredLastImage.crop();  // Creates a new processor.
    unalteredLastImage = unalteredLastImage.convertToFloat();
  }

  void fiddleImages() {
    // IJ.log("Fiddling the images");
    firstImage = unalteredFirstImage.crop();   // Creates a new processor.
    firstImage.setInterpolationMethod(ImageProcessor.BILINEAR);
    double shift = 0.5*(sourceStackImp.getWidth()-1) - centerPixel;
    firstImage.translate(shift, 0);
    firstImage.rotate(-detectorAngle);
    firstImage.setRoi(horizontalBorder,
                      topBorder,
                      firstImage.getWidth() - 2*horizontalBorder,
                      firstImage.getHeight() - topBorder - bottomBorder);
    firstImage = firstImage.crop();

    lastImage = unalteredLastImage.crop();   // Creates a new processor.
    lastImage.flipHorizontal();
    lastImage.setInterpolationMethod(ImageProcessor.BILINEAR);
    lastImage.translate(-shift, 0);
    lastImage.rotate(detectorAngle);
    lastImage.setRoi(horizontalBorder,
                     topBorder,
                     lastImage.getWidth() - 2*horizontalBorder,
                     lastImage.getHeight() - topBorder - bottomBorder);
    lastImage = lastImage.crop();
  }

  void showFirstAndLast() {
    // if (impFirstImage == null) {
    //   impFirstImage = sourceStackImp.createImagePlus();
    // }
    // impFirstImage.setProcessor("First Projection", firstImage);
    // impFirstImage.show();
    // 
    // if (impLastImage == null) {
    //   impLastImage = sourceStackImp.createImagePlus();
    // }
    // impLastImage.setProcessor("Last Projection", lastImage);
    // impLastImage.show();
  }

  void showOverlay() {
    ImageProcessor overlayImage = new ColorProcessor(firstImage.getWidth(), firstImage.getHeight());
    float firstImageMin = (float)(firstImage.getMin());
    float firstImageMax = (float)(firstImage.getMax());
    float lastImageMin = (float)(lastImage.getMin());
    float lastImageMax = (float)(lastImage.getMax());
    float firstImageScale = (float)(0xff)/(firstImageMax - firstImageMin);
    float lastImageScale = (float)(0xff)/(lastImageMax - lastImageMin);
    float[] firstImagePixels = (float[])firstImage.getPixels();
    float[] lastImagePixels = (float[])lastImage.getPixels();
    int[] overlayImagePixels = (int[])overlayImage.getPixels();
    int N = firstImagePixels.length;
    for (int i = 0; i < N; i++) {
      int firstImageComponent = (int)(firstImageScale * (firstImagePixels[i]-firstImageMin));
      int lastImageComponent = (int)(lastImageScale * (lastImagePixels[i]-lastImageMin));
      overlayImagePixels[i] =
        (firstImageComponent <<16) + (lastImageComponent << 8) + (lastImageComponent <<0);
    }
    if (impOverlayImage == null) {
      impOverlayImage = sourceStackImp.createImagePlus();
    }
    impOverlayImage.setProcessor("Projection overlay", overlayImage);
    impOverlayImage.show();
  }

  void computeCrossCorrelation() {
    double firstImageMean = 0;
    double firstImageStdDev = 0;
    double lastImageMean = 0;
    double lastImageStdDev = 0;
    double cc = 0;
    float[] firstImagePixels = (float[])firstImage.getPixels();
    float[] lastImagePixels = (float[])lastImage.getPixels();
    int N = firstImagePixels.length;
    double val;
    for (int i = 0; i < N; i++) {
      val = firstImagePixels[i];
      firstImageMean += val;
      firstImageStdDev += val*val;
    }
    firstImageMean /= N;
    firstImageStdDev = Math.sqrt(firstImageStdDev/N - firstImageMean*firstImageMean);
    for (int i = 0; i < N; i++) {
      val = lastImagePixels[i];
      lastImageMean += val;
      lastImageStdDev += val*val;
    }
    lastImageMean /= N;
    lastImageStdDev = Math.sqrt(lastImageStdDev/N - lastImageMean*lastImageMean);
    for (int i = 0; i < N; i++) {
      cc += (firstImagePixels[i]-firstImageMean)*(lastImagePixels[i]-lastImageMean);
    }
    cc /= N*firstImageStdDev*lastImageStdDev;
    crossCorrelation = cc;
		this.show();
  }
  
  public void reset() {
	  sourceStackImp = WindowManager.getCurrentImage();
	  if (sourceStackImp != null) {
      centerPixel = 0.5*(sourceStackImp.getWidth()-1);
      centerPixelText.setText(IJ.d2s(centerPixel, 6));
	  }
    // detectorAngle = 0;
    // detectorAngleText.setText("0");
    // horizontalBorder = 0;
    // horizontalBorderText.setText("0");
    // topBorder = 0;
    // topBorderText.setText("0");
    // bottomBorder = 0;
    // bottomBorderText.setText("0");
    
    if (impFirstImage != null) {
      impFirstImage.close();
      impFirstImage = null;
    }
    if (impLastImage != null) {
      impLastImage.close();
      impLastImage = null;
    }
    if (impOverlayImage != null) {
      impOverlayImage.close();
      impOverlayImage = null;
    }
    firstImage = null;
    lastImage = null;
  }

  final public MultivariateRealFunction partialDerivative(int k) {
    return null; // Not used
  }
  final public MultivariateVectorialFunction gradient() {
    return null; // Not used
  }

  // This method is called in the tune worker thread only
  // Not to be confused with run(String arg), which is a method of PlugInFrame
	public void run() {
	  
	  detectorAngle = Double.valueOf(detectorAngleText.getText()).doubleValue();
	  centerPixel = Double.valueOf(centerPixelText.getText()).doubleValue();
	  horizontalBorder = Integer.valueOf(horizontalBorderText.getText()).intValue();
	  topBorder = Integer.valueOf(topBorderText.getText()).intValue();
	  bottomBorder = Integer.valueOf(bottomBorderText.getText()).intValue();

    // IJ.log("Starting worker thread");
	  int count = 0;
	  double[] x = new double[2];
	  x[0] = centerPixel* tuningWeights[0];
	  x[1] = detectorAngle * tuningWeights[1];
    PowellOptimizer maximizer = new PowellOptimizer(1E-4);
    maximizer.setConvergenceChecker(new ConvergenceCheckerWithManualCancel(this,1E-4,1E-4));
    maximizer.setMaxEvaluations(1000000);
    maximizer.setMaxIterations(1000000);
	  try {
      // IJ.log("Starting optimization first round");
	    RealPointValuePair result =
	      maximizer.optimize(this, GoalType.MAXIMIZE, x);
      centerPixel = result.getPoint()[0] / tuningWeights[0];
      detectorAngle = result.getPoint()[1] / tuningWeights[1];
      centerPixelText.setText(IJ.d2s(centerPixel, 6));
      detectorAngleText.setText(IJ.d2s(detectorAngle, 6));
  	  crossCorrelation = result.getValue();
      updateCrossCorrelation();
      }
    catch (GetMeOuttaHereException e) {}
    catch (Exception e) {
      IJ.log("Exception occurred in optimizer.");
      stopTuning = true;
    }
    // Now do the whole thing again, but with narrower tolerances (the defaults, which are roughly machine precision)
    if (!stopTuning) {
      maximizer = new PowellOptimizer();
      maximizer.setConvergenceChecker(new ConvergenceCheckerWithManualCancel(this));
      maximizer.setMaxEvaluations(1000000);
      maximizer.setMaxIterations(1000000);
  	  try {
        // IJ.log("Starting optimization second round");
  	    RealPointValuePair result =
  	      maximizer.optimize(this, GoalType.MAXIMIZE, x);
        centerPixel = result.getPoint()[0] / tuningWeights[0];
        detectorAngle = result.getPoint()[1] / tuningWeights[1];
        centerPixelText.setText(IJ.d2s(centerPixel, 6));
        detectorAngleText.setText(IJ.d2s(detectorAngle, 6));
    	  crossCorrelation = result.getValue();
        updateCrossCorrelation();
        }
      catch (GetMeOuttaHereException e) {}
      catch (Exception e) {
        IJ.log("Exception occurred in optimizer.");
      }
    }
    
    UpdateOverlayAndControls();
    optimizeButton.setLabel("Optimize");
	  optimizeButton.setEnabled(true);
    updateButton.setEnabled(true);
    applyButton.setEnabled(true);
    resetButton.setEnabled(true);
	  detectorAngleText.setEnabled(true);
	  centerPixelText.setEnabled(true);
	  horizontalBorderText.setEnabled(true);
	  topBorderText.setEnabled(true);
    bottomBorderText.setEnabled(true);
    // IJ.log("Exiting worker thread");
	}

  void tune() {
    optimizeButton.setLabel("Stop Optimizing");
    updateButton.setEnabled(false);
    applyButton.setEnabled(false);
    resetButton.setEnabled(false);
	  detectorAngleText.setEnabled(false);
	  centerPixelText.setEnabled(false);
	  horizontalBorderText.setEnabled(false);
	  topBorderText.setEnabled(false);
	  bottomBorderText.setEnabled(false);
	  stopTuning = false;
    getFirstAndLast();
    tuningWeights = new double[2];
    tuningWeights[0] = 1;
    tuningWeights[1] = (Math.PI/180)*sourceStackImp.getWidth()/2.0;
		thread = new Thread(this);
		thread.start();
  }
  
  boolean getStopTuning() {
    return stopTuning;
  }
  
  // This probably needs to be better thought out:
  // Can't really exit until tuning quits.  Although possibly the
  // superclass includes a join command??
	public void close() {
		stopTuning = true;
		instance = null;
		super.close();
  }

  private void applyActionTheActionIsHere() {

    SaveDialog chooser = null;
    try {
      chooser = new SaveDialog("Save aligned image stack", "", ".mhd");
    } catch (Exception e) {
      return;
    }
    String headerName = chooser.getFileName();
    if ((headerName == null) || (headerName == "") || (headerName == ".mhd")) {
      return;
    }
    String directory = chooser.getDirectory();
    File headerPath = new File(directory, headerName);
    String rawDataName = null;
    if (headerName.endsWith(".mhd")) {
        final String nameRoot = headerName.substring(
                0, headerName.length() - ".mhd".length());
        rawDataName = nameRoot + ".raw";
    } else {
      IJ.error("Selected file name must end with .mhd");
      return;
    }

    File rawDataPath = new File(directory, rawDataName);
    FileOutputStream out;
    try {
      out = new FileOutputStream(rawDataPath);
    }
    catch (IOException e) {
      IJ.error("Error writing file " + rawDataPath.toString());
      return;
    }
    ImageStack stack = sourceStackImp.getStack();
    int stackSize = sourceStackImp.getStackSize();
    ImagePlus tempImp;
    FileInfo fileInfo = null;
    for (int s=1; s <= stackSize; s++) {
      IJ.showStatus("Processing slice " + Integer.toString(s) + "/" + Integer.toString(stackSize));
      IJ.showProgress(s,stackSize);
      ImageProcessor slice = stack.getProcessor(s);
      slice = slice.crop();  // Creates a new processor.
      slice.setInterpolationMethod(ImageProcessor.BILINEAR);
      double shift = 0.5*(sourceStackImp.getWidth()-1) - centerPixel;
      slice.translate(shift, 0);
      slice.rotate(-detectorAngle);
      slice.setRoi(horizontalBorder,
                        topBorder,
                        slice.getWidth() - 2*horizontalBorder,
                        slice.getHeight() - topBorder - bottomBorder);
      slice = slice.crop();
      ImagePlus sliceImp = new ImagePlus();
      sliceImp.setProcessor("Dummy", slice);
      fileInfo = sliceImp.getFileInfo();
      ImageWriter writer = new ImageWriter(fileInfo);
      try {
        writer.write(out);
      }
      catch (IOException e) {
        IJ.error("Error writing file " + headerPath.toString());
        return;
      }
    }
    try {
      out.close();
    }
    catch (IOException e) {
      IJ.error("Error writing file " + headerPath.toString());
      return;
    }
    IJ.showProgress(1);
    IJ.showStatus("done");
    
    // Now save the mhd file and show it.
    if (fileInfo != null) {
  		FileInfo inputFileInfo = new FileInfo();
      inputFileInfo.fileName = rawDataName;
      inputFileInfo.directory = directory;
  		inputFileInfo.fileFormat = inputFileInfo.RAW;
  		inputFileInfo.width = fileInfo.width;
  		inputFileInfo.height = fileInfo.height;
  		inputFileInfo.nImages = stackSize;
  		inputFileInfo.fileType = fileInfo.fileType;
  		inputFileInfo.intelByteOrder = fileInfo.intelByteOrder;
  		Calibration calibration = sourceStackImp.getCalibration();
  		inputFileInfo.pixelWidth = calibration.pixelWidth;
  		inputFileInfo.pixelHeight = calibration.pixelHeight;
  		inputFileInfo.pixelDepth = calibration.pixelDepth;
  		inputFileInfo.unit = calibration.getUnit();
      FileInfoVirtualStack virtualStack = new FileInfoVirtualStack(inputFileInfo, false);
      ImagePlus virtualStackImp = new ImagePlus(headerName, virtualStack);
      virtualStackImp.getCalibration().pixelWidth = calibration.pixelWidth;
      virtualStackImp.getCalibration().pixelHeight = calibration.pixelHeight;
      virtualStackImp.getCalibration().pixelDepth = calibration.pixelDepth;
      virtualStackImp.getCalibration().setUnit(calibration.getUnit());
      virtualStackImp.show();

      try {
        MiEncoder.writeHeader(virtualStackImp, headerPath.toString(),
                                      rawDataName);
      } catch (Exception e) {
        IJ.error("Error writing file " + headerPath.toString());
      }
    }

  }

  public void applyAction() {
    if (sourceStackImp == null || impOverlayImage == null) {
      return;
    }
    // IJ.log("Starting Apply");

    applyButton.setLabel("Processing. Please wait...");
    optimizeButton.setEnabled(false);
    updateButton.setEnabled(false);
    applyButton.setEnabled(false);
    resetButton.setEnabled(false);
	  detectorAngleText.setEnabled(false);
	  centerPixelText.setEnabled(false);
	  horizontalBorderText.setEnabled(false);
	  topBorderText.setEnabled(false);
	  bottomBorderText.setEnabled(false);

    applyActionTheActionIsHere();

    applyButton.setLabel("Apply to stack and save");
    optimizeButton.setEnabled(true);
    updateButton.setEnabled(true);
    applyButton.setEnabled(true);
    resetButton.setEnabled(true);
	  detectorAngleText.setEnabled(true);
	  centerPixelText.setEnabled(true);
	  horizontalBorderText.setEnabled(true);
	  topBorderText.setEnabled(true);
	  bottomBorderText.setEnabled(true);
  }
    
  
	public void actionPerformed(ActionEvent e) {
		String label = e.getActionCommand();
		if (label==null) {
			return;
		}
		if (label.equals("Update")) {
      updateAction();
    }
		else if (label.equals("Optimize")) {
		  tune();
	  }
		else if (label.equals("Stop Optimizing")) {
		  optimizeButton.setLabel("Stopping. Please wait...");
		  optimizeButton.setEnabled(false);
	    stopTuning = true;
    }
		else if (label.equals("Apply to stack and save"))
      applyAction();
		else if (label.equals("Reset"))
      reset();
	}

}


class ConvergenceCheckerWithManualCancel implements RealConvergenceChecker {

    /** Default relative threshold. */
    private static final double DEFAULT_RELATIVE_THRESHOLD = 100 * MathUtils.EPSILON;

    /** Default absolute threshold. */
    private static final double DEFAULT_ABSOLUTE_THRESHOLD = 100 * MathUtils.SAFE_MIN;

    /** Relative tolerance threshold. */
    private final double relativeThreshold;

    /** Absolute tolerance threshold. */
    private final double absoluteThreshold;

    private Align_Projections apObject;

   /** Build an instance with default threshold.
     */
    public ConvergenceCheckerWithManualCancel(Align_Projections apObject) {
        this.relativeThreshold = DEFAULT_RELATIVE_THRESHOLD;
        this.absoluteThreshold = DEFAULT_ABSOLUTE_THRESHOLD;
        this.apObject = apObject;
    }

    /** Build an instance with a specified threshold.
     * <p>
     * In order to perform only relative checks, the absolute tolerance
     * must be set to a negative value. In order to perform only absolute
     * checks, the relative tolerance must be set to a negative value.
     * </p>
     * @param relativeThreshold relative tolerance threshold
     * @param absoluteThreshold absolute tolerance threshold
     */
    public ConvergenceCheckerWithManualCancel(Align_Projections apObject,
          final double relativeThreshold,
          final double absoluteThreshold
          ) {
        this.relativeThreshold = relativeThreshold;
        this.absoluteThreshold = absoluteThreshold;
        this.apObject = apObject;
    }

    public boolean converged(final int iteration,
                             final RealPointValuePair previous,
                             final RealPointValuePair current) {
        if (apObject.getStopTuning()) {
          return true;
        }
        final double p          = previous.getValue();
        final double c          = current.getValue();
        final double difference = FastMath.abs(p - c);
        final double size       = FastMath.max(FastMath.abs(p), FastMath.abs(c));
        return (difference <= (size * relativeThreshold)) || (difference <= absoluteThreshold);
    }
}

class GetMeOuttaHereException extends RuntimeException {}
