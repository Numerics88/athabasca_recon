VERSION_TEXT = 1.3
PDF_MANUAL_NAME = athabasca_recon_manual_v$(VERSION_TEXT).pdf

html: athabasca_recon_manual.adoc
	asciidoctor \
		athabasca_recon_manual.adoc

pdf: athabasca_recon_manual.adoc
	asciidoctor -b docbook5 \
		athabasca_recon_manual.adoc
	dblatex -t pdf -p /etc/asciidoc/dblatex/asciidoc-dblatex.xsl \
	    -s asciidoc-dblatex.sty \
	    -P latex.output.revhistory=0 \
	    -P doc.publisher.show=0 \
	    athabasca_recon_manual.xml
	mv athabasca_recon_manual.pdf $(PDF_MANUAL_NAME)
