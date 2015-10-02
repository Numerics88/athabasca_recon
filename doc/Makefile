VERSION_TEXT = 1.3
PDF_MANUAL_NAME = athabasca_recon_manual_v$(VERSION_TEXT).pdf

html: athabasca_recon_manual.txt
	asciidoc -a toc -a icons -a numbered -a toclevels=4 athabasca_recon_manual.txt

pdf: athabasca_recon_manual.txt
	a2x -f pdf \
    --asciidoc-opts "-a docinfo" \
    --dblatex-opts "-s asciidoc-dblatex.sty"\
    --dblatex-opts "-P latex.output.revhistory=0 -P doc.publisher.show=0" \
		athabasca_recon_manual.txt
		mv athabasca_recon_manual.pdf $(PDF_MANUAL_NAME)
