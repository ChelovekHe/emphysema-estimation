TEX=pdflatex

TEX_FILES=README.tex

PLOTS=../Plots/Stability-1_DistanceDistribution.pdf\
	../Plots/Stability-2_DistanceDistribution.pdf\
	../Plots/Stability-3_DistanceDistribution.pdf\
	../Plots/Stability-4_DistanceDistribution.pdf\
	../Plots/Stability-5_DistanceDistribution.pdf

MAIN_TEX_FILE=StabilityOfClusteringAlgorithmReport.tex

report: ${MAIN_TEX_FILE}
	${TEX} $^	

StabilityOfClusteringAlgorithmReport.tex: ${TEX_FILES} ${PLOTS}

plots: ${PLOTS}

README.tex: ../README.md
	pandoc -o $@ $^

%.pdf : %.ps
	ps2pdf $^ $@
