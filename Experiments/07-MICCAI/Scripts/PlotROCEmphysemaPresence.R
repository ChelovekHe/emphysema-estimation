#!/usr/bin/Rscript
main <- function(options) {
    if ( length(options) < 4 ) {
        print( "Usage: <reference-scores> <predicted> <output-path> <title>" );
        quit( status = 1 );
    }

    PlotROC( options[1], options[2], options[3], options[4] );
    PlotROCRegions( options[1], options[2], options[3], options[4] );
    PlotROCLeftRight( options[1], options[2], options[3], options[4] );
    quit( status = 0 );
}

PlotROC <- function(visualScoresPath, predictionsPath, outpath, plottitle) {
    library(ROCR);
    visualScores <- read.csv(visualScoresPath, header=TRUE);
    predictions <- read.csv(predictionsPath, header=TRUE);
    scores <- merge(visualScores, predictions, by='IdDate');

    predicted <- scores[, c("ERU", "ERM", "ERL", "ELU", "ELM", "ELL")]
    predicted <- rowSums(scores[, c("ERU", "ERM", "ERL", "ELU", "ELM", "ELL")])/6;
    target.L1 <-  apply(scores[, c("ERU.L1", "ERM.L1", "ERL.L1", "ELU.L1", "ELM.L1", "ELL.L1")], 1, max) > 1
    pred.L1 <- prediction(predicted, target.L1);
    perf.L1 <- performance(pred.L1, measure="tpr", x.measure="fpr");
    auc.L1 <- performance(pred.L1, measure="auc");

    target.M1 <-  apply(scores[, c("ERU.M1", "ERM.M1", "ERL.M1", "ELU.M1", "ELM.M1", "ELL.M1")], 1, max) > 1
    pred.M1 <- prediction(predicted, target.M1);
    perf.M1 <- performance(pred.M1, measure="tpr", x.measure="fpr");
    auc.M1 <- performance(pred.M1, measure="auc");
    
    pdf(file=paste(outpath, "Lungs_ROC.pdf", sep='_'));
    plot(perf.L1,
         main=plottitle,
         col='blue'
         );    
    plot(perf.M1, add=T, col='red' );
    legend(x="bottomright",
           legend=c(paste("L1", round(auc.L1@y.values[[1]], 2)),
               paste('M1', round(auc.M1@y.values[[1]], 2))),
           lty=1,
           col=c('blue', 'red')
           )
    dev.off()    
}

PlotROCRegions <- function(visualScoresPath, predictionsPath, outpath, plottitle) {
    library(ROCR);
    visualScores <- read.csv(visualScoresPath, header=TRUE);
    predictions <- read.csv(predictionsPath, header=TRUE);
    scores <- merge(visualScores, predictions, by='IdDate');

    plottitle <- paste(plottitle, 'in regions')
    
    regions <- c("ERU", "ERM", "ERL", "ELU", "ELM", "ELL");
    raters <- c("L1", "M1")
    for (rater in raters) {
        pdf(file=paste(outpath, '_Regions_', rater, '_ROC.pdf', sep=''));
        i <- 1
        addPlot <- FALSE
        legends <- c();
        for ( region in regions ) {
            predicted <- scores[, region]
            target <-  scores[, paste(region, rater, sep='.')] > 1
            pred <- prediction(predicted, target)
            perf <- performance(pred, measure="tpr", x.measure="fpr")
            legends <- c(legends, paste(region, round(performance(pred, measure="auc")@y.values[[1]], 2)))
            plot(perf, col=i, add=addPlot )
            i <- i + 1
            if ( !addPlot ) {
                addPlot <- TRUE
            }
        }
        title(main=paste(plottitle, rater))
        legend(x="bottomright",
               legend=legends,
               col=1:6,
               lty=1)
        dev.off()
    }
}

PlotROCLeftRight <- function(visualScoresPath, predictionsPath, outpath, plottitle) {
    library(ROCR);
    visualScores <- read.csv(visualScoresPath, header=TRUE);
    predictions <- read.csv(predictionsPath, header=TRUE);
    scores <- merge(visualScores, predictions, by='IdDate');

    plottitle <- paste(plottitle, 'in left/right lung')

    predicted.R <- rowSums(scores[, c("ERU", "ERM", "ERL")])/3
    target.R.L1 <-  apply(scores[, c("ERU.L1", "ERM.L1", "ERL.L1")], 1, max) > 1
    pred.R.L1 <- prediction(predicted.R, target.R.L1);
    perf.R.L1 <- performance(pred.R.L1, measure="tpr", x.measure="fpr");
    auc.R.L1 <- performance(pred.R.L1, measure="auc");
                          
    predicted.L <- rowSums(scores[, c("ELU", "ELM", "ELL")])/3                                     
    target.L.L1 <-  apply(scores[, c("ELU.L1", "ELM.L1", "ELL.L1")], 1, max) > 1
    pred.L.L1 <- prediction(predicted.L, target.L.L1);
    perf.L.L1 <- performance(pred.L.L1, measure="tpr", x.measure="fpr");
    auc.L.L1 <- performance(pred.L.L1, measure="auc");

    
    target.R.M1 <-  apply(scores[, c("ERU.M1", "ERM.M1", "ERL.M1")], 1, max) > 1
    pred.R.M1 <- prediction(predicted.R, target.R.M1);
    perf.R.M1 <- performance(pred.R.M1, measure="tpr", x.measure="fpr");
    auc.R.M1 <- performance(pred.R.M1, measure="auc");
                          
    target.L.M1 <-  apply(scores[, c("ELU.M1", "ELM.M1", "ELL.M1")], 1, max) > 1
    pred.L.M1 <- prediction(predicted.L, target.L.M1);
    perf.L.M1 <- performance(pred.L.M1, measure="tpr", x.measure="fpr");
    auc.L.M1 <- performance(pred.L.M1, measure="auc");
        
    pdf(file=paste(outpath, "LungsLeftRight_ROC.pdf", sep='_'));
    plot(perf.R.L1, main=plottitle, col=1 );
    plot(perf.L.L1, add=T, col=1, lty=2 );
    plot(perf.R.M1, add=T, col=2 );
    plot(perf.L.M1, add=T, col=2, lty=2 );
   
    legend(x="bottomright",
           legend=c(
               paste("R.L1", round(auc.R.L1@y.values[[1]], 2)),
               paste("L.L1", round(auc.L.L1@y.values[[1]], 2)),
               paste("R.M1", round(auc.R.M1@y.values[[1]], 2)),
               paste("L.M1", round(auc.L.M1@y.values[[1]], 2))),
           lty=c(1,2,1,2),
           col=c(1,1,2,2)
           )
    dev.off()    

}


options <- commandArgs(trailingOnly=TRUE);
main(options);
