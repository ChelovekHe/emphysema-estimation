TEX=pdflatex

TEX_FILES=README.tex

PLOTS=../Plots/Scalability-1_Runtime.pdf\
	../Plots/Scalability-2_Runtime.pdf

MAIN_TEX_FILE=ScalabilityOfClusteringAlgorithmReport.tex

report: ${MAIN_TEX_FILE}
	${TEX} $^	

ScalabilityOfClusteringAlgorithmReport.tex: ${TEX_FILES} ${PLOTS}

plots: ${PLOTS}

README.tex: ../README.md
	pandoc -o $@ $^

%.pdf : %.ps
	ps2pdf $^ $@
