#!/usr/bin/Rscript

# Plot scalability stats and save to file

main <- function(options) {
    if ( length(options) < 3 ) {
        print( "Usage: <distances-path> <output-path> <title>" );
        quit( status = 1 );
    }
    PlotStabilityDistances( options[1], options[2], options[3] );
    quit( status = 0 );
}

PlotStabilityDistances <- function(inpath,outpath,plottitle) {
    dists <- read.csv(inpath);
    ks <- unique(dists$k);
    plot.dists <- list();
    for (k in ks ) {
        plot.dists[[paste('k',k)]] <- dists[dists$k == k, "distance"];
    }

    postscript(file=outpath, bg="white" );
    boxplot(plot.dists,
            names=ks,
            xlab="k",
            ylab="Hausdorff distance",
            main=plottitle
            );
    dev.off();
}


options <- commandArgs(trailingOnly=TRUE);
main(options);
