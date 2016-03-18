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
    visualScores <- read.csv(visualScoresPath, header=TRUE);
    predictions <- read.csv(predictionsPath, header=TRUE);

    scores <- merge(visualScores, predictions, by='IdDate');
    
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

    L1M1 <- (L1 + M1)/2;

    
    printRegion <- substr(region,2,3);

    rho <- round(cor(cbind(L1,M1), method='spearman'),3)[1,2]
    print( rho )
    limMax <- max(L1,M1)
    print(limMax)
    pdf(file=paste(outPrefix, 'R1vsR2.pdf', sep=''))
    plot(L1,
         M1,
         main=paste('Emphysema extent', printRegion, "- R1 vs R2. rho =", rho),
         xlim=c(0,limMax),
         ylim=c(0,limMax),
         col='red'
         )
    lines(c(0,limMax), c(0,limMax), col='black')
    dev.off()

    rho <- round(cor(cbind(L1M1,CMS), method='spearman'),3)[1,2]
    limMax <- max(L1M1,CMS)
    print(limMax)
    pdf(file=paste(outPrefix, 'R1R2vsCMS.pdf', sep=''))
    plot(L1M1,
         CMS,
         main=paste('Emphysema extent', printRegion, "- Avg vs CMS. rho =", rho),
         xlab="Avg",
         xlim=c(0,limMax),
         ylim=c(0,limMax),
         col='red'
         )
    lines(c(0,limMax), c(0,limMax), col='black')
    dev.off()

    rho <- round(cor(cbind(L1,CMS), method='spearman'),3)[1,2]
    limMax <- max(L1,CMS)
    print(limMax)
    pdf(file=paste(outPrefix, 'R1vsCMS.pdf', sep=''))
    plot(L1,
         CMS,
         main=paste('Emphysema extent',printRegion, "- R1 vs CMS. rho =", rho),
         xlim=c(0,limMax),
         ylim=c(0,limMax),
         col='red'
         )
    lines(c(0,limMax), c(0,limMax), col='black')
    dev.off()

    rho <- round(cor(cbind(M1,CMS), method='spearman'),3)[1,2]
    limMax <- max(M1,CMS)
    print(limMax)
    pdf(file=paste(outPrefix, 'R2vsCMS.pdf', sep=''))
    plot(M1,
         CMS,
         main=paste('Emphysema extent',printRegion, "- R2 vs CMS. rho =", rho),
         xlim=c(0,limMax),
         ylim=c(0,limMax),
         col='red'
         )
    lines(c(0,limMax), c(0,limMax), col='black')
    dev.off()
}


options <- commandArgs(trailingOnly=TRUE);
main(options);
