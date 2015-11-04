#!/usr/bin/Rscript

# Plot entropy stats and save to file

main <- function(options) {
    if ( length(options) < 3 ) {
        print( "Usage: <entropies-path> <output-path> <title> [<normalization>]" );
        quit( status = 1 );
    }
    normalization = 1;
    if ( length(options) >= 4 ) {
        normalization = as.numeric(options[4]);
    }
    PlotEntropies( options[1], options[2], options[3], normalization );
    quit( status = 0 );
}

PlotEntropies <- function(inpath, outpath, plottitle, normalization) {
    entropy <- read.csv(inpath, header=FALSE);    
    ## Each column corresponds to one cluster
    ## Each row corresponds to one measurement
    print(max(entropy))
    print(max(entropy/normalization))
    k <- ncol(entropy);
    pdf(file=outpath, bg="white" );
    boxplot(entropy/normalization,
            ylim=c(0,1),
            names=seq(1,k),
            xlab="Clusters",
            ylab="Entropy",
            main=plottitle
            );
    dev.off();
}


options <- commandArgs(trailingOnly=TRUE);
main(options);
