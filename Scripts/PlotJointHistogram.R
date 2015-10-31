#!/usr/bin/Rscript

# Plot a joint histogram and save it as a postscript file

main <- function(options) {
    if ( length(options) < 3 ) {
        print( "Usage: <histogram-path> <output-path> <title>" );
        quit( status = 1 );
    }
    PlotJointHistogram( options[1], options[2], options[3] );
    quit( status = 0 );
}



PlotJointHistogram <- function(inpath,outpath,title) {
    data <- read.table(inpath, header=F);
    labels.y <- rev(as.character(data[-1,1]));
    labels.x <- as.character(data[1,-1]);
    nLabels <- 10;
    imdata <- as.matrix(data[-1,-1]);

    png(file=outpath, bg="white" );
    labels.y.idx <- round(seq(1, length(labels.y), length(labels.y)/nLabels));
    labels.x.idx <- round(seq(1, length(labels.x), length(labels.x)/nLabels));
    par('mar'=c(5,5,5,5))
    filled.contour(
        imdata,
        color.palette=topo.colors,
        plot.title=title(main=title, ylab="  ", xlab="Calculated"),
        plot.axes= {
            axis(1, at=seq(0,1,by=1/(nLabels-1)), labels=labels.x[labels.x.idx]);
            axis(2, at=seq(0,1,by=1/(nLabels-1)), labels=rev(labels.y[labels.y.idx]))
        }
    );
    dev.off();
}

options <- commandArgs(trailingOnly=TRUE);
main(options);
