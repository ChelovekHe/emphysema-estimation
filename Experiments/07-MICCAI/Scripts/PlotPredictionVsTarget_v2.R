#!/usr/bin/Rscript

main <- function(options) {
    if ( length(options) < 4 ) {
        print( "Usage: <visual-scores> <predictions> <region> <out-prefix>" );
        quit( status = 1 );
    }
       
    PlotPredictionVsTarget( options[1], options[2], options[3], options[4] );
    quit( status = 0 );
}

PlotPredictionVsTarget <- function(visualScoresPath, predictionsPath, region, outPrefix) {
    printRegion <- substr(region,2,3);
    
    visualScores <- read.csv(visualScoresPath, header=TRUE);
    predictions <- read.csv(predictionsPath, header=TRUE);

    scores <- merge(visualScores, predictions, by='IdDate');

    intervals.low <-  c(0.01, 0.06, 0.26, 0.51, 0.76)
    intervals.high <- c(0.05, 0.25, 0.50, 0.75, 1.00)
    
    L1 <- scores[, paste(region, "L1", sep=".")]
    L1[L1 <= 1] <- 0;
    L1[L1 == 2] <- (0.01 + 0.05)/2;
    L1[L1 == 3] <- (0.06 + 0.25)/2;
    L1[L1 == 4] <- (0.26 + 0.50)/2;
    L1[L1 == 5] <- (0.51 + 0.75)/2;
    L1[L1 == 6] <- (0.76 + 1.00)/2;
    
    M1 <- scores[, paste(region, "M1", sep=".")]
    M1[M1 <= 1] <- 0;
    M1[M1 == 2] <- (0.01 + 0.05)/2;
    M1[M1 == 3] <- (0.06 + 0.25)/2;
    M1[M1 == 4] <- (0.26 + 0.50)/2;
    M1[M1 == 5] <- (0.51 + 0.75)/2;
    M1[M1 == 6] <- (0.76 + 1.00)/2;
    
    CMS <- round(scores[, region], 2);
    ## CMS[0.01 <= CMS & CMS <= 0.05] <- (0.01 + 0.05)/2;
    ## CMS[0.06 <= CMS & CMS <= 0.25] <- (0.06 + 0.25)/2;
    ## CMS[0.26 <= CMS & CMS <= 0.50] <- (0.26 + 0.50)/2;
    ## CMS[0.51 <= CMS & CMS <= 0.75] <- (0.51 + 0.75)/2;
    ## CMS[0.76 <= CMS & CMS <= 1.00] <- (0.76 + 1.00)/2;

    pdf(file=paste(outPrefix, 'L1vsM1.pdf', sep=''))
    plot(L1,
         M1,
         main=paste("L1 vs M1.", printRegion),
         xlim=c(0,1),
         ylim=c(0,1),
         col='red'
         )
    lines(c(0,1), c(0,1), col='black')
    dev.off()
        
    pdf(file=paste(outPrefix, 'L1vsCMS.pdf', sep=''))
    plot(L1,
         CMS,
         main=paste("L1 vs CMS.", printRegion),
         xlim=c(0,1),
         ylim=c(0,1),
         col='red'
         )
    lines(c(0,1), c(0,1), col='black')
    for ( i in 1:length(intervals.low) ) {
        low <- intervals.low[i]
        high <- intervals.high[i]
        lines(c(0,1), c(low,low),  col=2+i )
    }
    dev.off()

    pdf(file=paste(outPrefix, 'M1vsCMS.pdf', sep=''))
    plot(M1,
         CMS,
         main=paste("M1 vs CMS.", printRegion),
         xlim=c(0,1),
         ylim=c(0,1),
         col=1
         )
    lines(c(0,1), c(0,1), col='black')
    dev.off()
}


options <- commandArgs(trailingOnly=TRUE);
main(options);
