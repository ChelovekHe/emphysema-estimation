#!/usr/bin/Rscript
main <- function(options) {
    if ( length(options) < 5 ) {
        print( "Usage: <reference-scores> <predicted> <region> <output-path> <title>" );
        quit( status = 1 );
    }

    PlotROCRegion( options[1], options[2], options[3], options[4], options[5] );
    quit( status = 0 );
}

PlotROCRegion <- function(visualScoresPath, predictionsPath, region, outpath, plottitle) {
    library(ROCR);
    visualScores <- read.csv(visualScoresPath, header=TRUE);
    predictions <- read.csv(predictionsPath, header=TRUE);
    scores <- merge(visualScores, predictions, by='IdDate');

    plottitle <- paste(plottitle, 'in regions')
    
    pdf(file=paste(outpath, '_Regions_ROC.pdf', sep=''));

    predicted <- scores[, region]
    target.L1 <- scores[, paste(region, 'L1' , sep='.')] > 1
    target.M1 <- scores[, paste(region, 'M1' , sep='.')] > 1
    target.LM <- pmax(scores[, paste(region, 'L1', sep='.')],
                      scores[, paste(region, 'M1' , sep='.')]) > 1

    pred.L1 <- prediction(predicted, target.L1)
    perf.L1 <- performance(pred.L1, measure="tpr", x.measure="fpr")
    pred.M1 <- prediction(predicted, target.M1)
    perf.M1 <- performance(pred.M1, measure="tpr", x.measure="fpr")
    pred.LM <- prediction(predicted, target.LM)
    perf.LM <- performance(pred.LM, measure="tpr", x.measure="fpr")

    auc.L1 <- round(performance(pred.L1, measure="auc")@y.values[[1]], 2)
    auc.M1 <- round(performance(pred.M1, measure="auc")@y.values[[1]], 2)
    auc.LM <- round(performance(pred.LM, measure="auc")@y.values[[1]], 2)
    
    plot( perf.LM, col=1, main=plottitle )
    plot( perf.L1, col=2, add=T )
    plot( perf.M1, col=3, add=T )
    
    legend(x="bottomright",
           legend=paste(c('Avg', 'R1', 'R2'), c(auc.LM, auc.L1, auc.M1)),
           col=1:6,
           lty=1)
    dev.off()
}


options <- commandArgs(trailingOnly=TRUE);
main(options);
