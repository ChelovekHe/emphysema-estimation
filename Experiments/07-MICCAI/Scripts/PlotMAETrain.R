#!/usr/bin/Rscript

main <- function(options) {
    num.options <- length( options );
    n <- 193;
    
    if ( num.options < 3 ) {
        print( "Usage: <sae> <title> <out>" );
        quit( status = 1 );
    }

    plot.title <- options[num.options - 1];
    out.path <- options[num.options];

    regions <- c("ERU", "ERM", "ERL", "ELU", "ELM", "ELL")
    
    sae <- read.csv( options[1], header=T );
    mae <- sae[, regions] / n;
    pdf( file=out.path )
    plot(x=c(),
         xlim=c(min(sae$k), max(sae$k)),
         ylim=c(0, max(mae)),
         xlab="#Clusters",
         ylab="MAE",
         main=plot.title);
    i <- 1
    for ( region in  regions) {
        points( sae$k, mae[,region], col=i );
        lines( sae$k, mae[,region], col=i );
        i <- i + 1
    }
    legend(x="topright",
           legend=regions,
           col=c(1:6),
           lty=1);
    dev.off()
    quit( status = 0 );
}

options <- commandArgs(trailingOnly=TRUE);
main(options);
