#!/usr/bin/Rscript

# Plot scalability stats and save to file

main <- function(options) {
    if ( length(options) < 3 ) {
        print( "Usage: <stats-path> <output-path> <title>" );
        quit( status = 1 );
    }
    PlotScalabilityStats( options[1], options[2], options[3] );
    quit( status = 0 );
}


PlotScalabilityStats <- function(inpath,outpath,title) {
    stats <- read.csv(inpath, header=TRUE);
    stats$meanTime <- stats$meanTime/1000;
    
    # We want a plot with one line for each k
    ks <- unique(stats$k);
    postscript(file=outpath, bg="white");
    plot(1,
         xlim=range(stats$numSamples),
         ylim=range(stats$meanTime),
#             (stats$meanTime - stats$stddevTime)/1000,
 #            (stats$meanTime + stats$stddevTime)/1000)),
         xlab="Number of samples",
         ylab="Averatge runtime in seconds",
         main=title,
         xaxt="n",
         );
    axis(1, at=unique(stats$numSamples));
    i = 1;
    for ( k in ks ) {
        k.stats <- stats[stats$k == k,];
        numSamples <- k.stats$numSamples;
        meanTime <- k.stats$meanTime;
        #stddevTime <- k.stats$stddevTime/1000;
        
        points(numSamples,
               meanTime,
               pch=i,
               );
        lines( numSamples, meanTime, col=i );
        i <- i + 1;
        # We cant see the stddev
        # hack: we draw arrows but with very special "arrowheads"
        #arrows(numSamples, meanTime-stddevTime,
         #      numSamples, meanTime+stddevTime,
          #     length=0.05, angle=90, code=3);

    }
    legend(x=min(stats$numSamples),
           y=max(stats$meanTime),
           legend=ks,
           title="Number of clusters",
           col=1:(length(ks)),
           lty=1,
           );
    dev.off();
}

options <- commandArgs(trailingOnly=TRUE);
main(options);
