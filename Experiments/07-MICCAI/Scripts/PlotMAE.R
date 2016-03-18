#!/usr/bin/Rscript

main <- function(options) {
    num.options <- length( options );
    
    if ( num.options < 4 ) {
        print( "Usage: <target> <predictions>  [<predictions>] <title> <out>" );
        quit( status = 1 );
    }

    plot.title <- options[num.options - 1];
    out.path <- options[num.options];
    
    target <- read.csv( options[1], header=T );

    mae <- data.frame(numeric(),numeric(),numeric(),numeric(),numeric(),numeric() );
    for ( path in options[2:(num.options-2)] ) {
        prediction <- read.csv( path, header=T );
        mae <- rbind( mae, calculateMAE(target, prediction) );
    }
    colnames(mae) <- c("k", "ERU", "ERM", "ERL", "ELU", "ELM", "ELL");
    print( mae )


    pdf( file=out.path )
    plot(x=c(),
         xlim=c(min(mae$k), max(mae$k)),
         ylim=c(0, max(mae[,-1])),
         xlab="#Clusters",
         ylab="MAE",
         main=plot.title);
    i <- 1
    regions <- c('ELL', 'ELM', 'ELU', 'ERL', 'ERM', 'ERU')
    for ( region in regions ) {
        lines( mae$k, mae[,region], col=i, lwd=3 );
        points( mae$k[which.min(mae[,region])], min(mae[,region]), pch=21, col=i, bg=i)
        i <- i + 1
    }
    legend(x="topright",
           legend=substr(regions, 2, 3),
           col=c(1:6),
           lty=1,
           lwd=3);
    dev.off()
    quit( status = 0 );
}

calculateMAE <- function(target, prediction) {
    scores <- merge(target, prediction, by="IdDate")
    k <- unique( scores$k )
    if (length(k) == 1) {
        low.I <- c("low.ERU", "low.ERM", "low.ERL", "low.ELU", "low.ELM", "low.ELL")
        high.I <- c("high.ERU", "high.ERM", "high.ERL", "high.ELU", "high.ELM", "high.ELL")
        pred.I <- c("ERU", "ERM", "ERL", "ELU", "ELM", "ELL")
        
        low <- scores[,low.I]
        high <- scores[,high.I]
        pred <- scores[,pred.I]
        
        low.ae <- abs( low - pred )
        high.ae <- abs( high - pred )
        ae <- pmin(low.ae, high.ae);
        ae[pred >= low & pred <= high] <- 0; # Set to zero inside interval
        c(k, round( colMeans(ae), 3 ));
    }
    else {
        c(NaN, NaN, NaN, NaN, NaN, NaN, NaN);
    }
                     
}


options <- commandArgs(trailingOnly=TRUE);
main(options);
