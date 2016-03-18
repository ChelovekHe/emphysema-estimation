#!/usr/bin/Rscript

main <- function(options) {
    if ( length(options) < 3 ) {
        print( "Usage: <inpath> <output-path> <title>" );
        quit( status = 1 );
    }

    PlotErrors( options[1], options[2], options[3] );
    quit( status = 0 );
}

PlotErrors <- function(inpath, outpath, plottitle) {
    res <- read.table(inpath, header=TRUE);
    pdf(file=outpath);
    upper <- max(res$target, res$prediction);
    rho <- round(cor(res, method='spearman')['target', 'prediction'],3)
    plot( res$target, res$prediction,
         xlab="Target", ylab="Predicted",
         xlim=c(0,upper), ylim=c(0,upper),
         col=2,
         main=paste(plottitle, 'rho =', rho )
         );
    lines(c(0,upper), c(0,upper), col=1);
    dev.off();

}


options <- commandArgs(trailingOnly=TRUE);
main(options);
