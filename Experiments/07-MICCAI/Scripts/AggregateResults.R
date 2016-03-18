#!/usr/bin/Rscript

main <- function(options) {
    if ( length(options) < 2 ) {
        print( "Usage: <visual-scores> <predictions>" );
        quit( status = 1 );
    }

    AggregateResults( options[1], options[2] );
    quit( status = 0 );
}

AggregateResults <- function(visualScoresPath, predictionsPath) {
    library(irr)
    visualScores <- read.csv(visualScoresPath, header=TRUE);
    predictions <- read.csv(predictionsPath, header=TRUE);

    scores <- merge(visualScores, predictions, by='IdDate');

    ## We can do a couple of things
    ## 1. Calculate the mean squared error used for the optimization. Since we
    ##    are dealing with intervals, we can calculate it in a couple of
    ##    different ways.
    ##    a) Use the mean of reference midpoints as true proportion
    ##
    ##    b) Set the error to zero for all predictions that lie in one of the
    ##       intervals, and use smallest distance from any endpoint for
    ##       predictions that dont lie in the interval.
    ##
    ##    c) Calculate the mean of the error over both references
    ##
    ## We try to combine b) and c) and see what we get

    ## Rater L1
    low.L1 <- scores[, c("ERU.L1", "ERM.L1", "ERL.L1", "ELU.L1", "ELM.L1", "ELL.L1")]
    low.L1[low.L1 <= 1] <- 0;
    low.L1[low.L1 == 2] <- 0.01;
    low.L1[low.L1 == 3] <- 0.06;
    low.L1[low.L1 == 4] <- 0.26;
    low.L1[low.L1 == 5] <- 0.51;
    low.L1[low.L1 == 6] <- 0.76;

    high.L1 <- scores[, c("ERU.L1", "ERM.L1", "ERL.L1", "ELU.L1", "ELM.L1", "ELL.L1")]
    high.L1[high.L1 <= 1] <- 0;
    high.L1[high.L1 == 2] <- 0.05;
    high.L1[high.L1 == 3] <- 0.25;
    high.L1[high.L1 == 4] <- 0.50;
    high.L1[high.L1 == 5] <- 0.75;
    high.L1[high.L1 == 6] <- 1;

    pred <- round(scores[, c("ERU", "ERM", "ERL", "ELU", "ELM", "ELL")], 2);
    low.ae.L1 <- abs(low.L1 - pred);
    high.ae.L1 <- abs(high.L1 - pred);
    ae.L1 <- pmin(low.ae.L1, high.ae.L1)
    ae.L1[pred >= low.L1 & pred <= high.L1] <- 0;
    ae.L1 <- cbind(ae.L1, ALL.L1=rowMeans(ae.L1));
    mae.L1 <- round(colMeans(ae.L1), 3);    

    ## Rater M1
    low.M1 <- scores[, c("ERU.M1", "ERM.M1", "ERL.M1", "ELU.M1", "ELM.M1", "ELL.M1")]
    low.M1[low.M1 <= 1] <- 0;
    low.M1[low.M1 == 2] <- 0.01;
    low.M1[low.M1 == 3] <- 0.06;
    low.M1[low.M1 == 4] <- 0.26;
    low.M1[low.M1 == 5] <- 0.51;
    low.M1[low.M1 == 6] <- 0.76;

    high.M1 <- scores[, c("ERU.M1", "ERM.M1", "ERL.M1", "ELU.M1", "ELM.M1", "ELL.M1")]
    high.M1[high.M1 <= 1] <- 0;
    high.M1[high.M1 == 2] <- 0.05;
    high.M1[high.M1 == 3] <- 0.25;
    high.M1[high.M1 == 4] <- 0.50;
    high.M1[high.M1 == 5] <- 0.75;
    high.M1[high.M1 == 6] <- 1;

    pred <- round(scores[, c("ERU", "ERM", "ERL", "ELU", "ELM", "ELL")], 2);
    low.ae.M1 <- abs(low.M1 - pred);
    high.ae.M1 <- abs(high.M1 - pred);
    ae.M1 <- pmin(low.ae.M1, high.ae.M1)
    ae.M1[pred >= low.M1 & pred <= high.M1] <- 0;
    ae.M1 <- cbind(ae.M1, ALL.M1=rowMeans(ae.M1));
    mae.M1 <- round(colMeans(ae.M1), 3);

    ## Average over both raters
    mae <- round((mae.L1 + mae.M1)/2, 3);
    names( mae ) <- c("ERU", "ERM", "ERL", "ELU", "ELM", "ELL", "ALL")

    cat('=============== Mean Absolute Error ==============\n');
    cat( '\t\tL1/CMS\n' );
    print( mae.L1 );    
    cat( '\n\t\tM1/CMS\n' );
    print( mae.M1 );
    cat( '\n\tmean(L1/CMS, M1/CMS)\n' );
    print( mae )
    

    ## 2. We can calculate intraclass correlation coefficient
    ## L1 <- (low.L1 + high.L1)/2;
    ## M1 <- (low.M1 + high.M1)/2;
    ## ERU.ref <- cbind(L1$ERU.L1, M1$ERU.M1)
    ## print(icc(ERU.ref, model="twoway", type="c"))
    ## ERU <- cbind(ERU.ref, scores$ERU);
    ## print(icc(ERU, model="twoway", type="c"))
    
    
    ## 3. We can look at contingency tables
    classes <- scores; 
    x <- round(classes[, c("ERU", "ERM", "ERL", "ELU", "ELM", "ELL")], 2);
    x[x >= 0.76]            <- 6;
    x[x >= 0.51 & x < 0.76] <- 5;
    x[x >= 0.26 & x < 0.51] <- 4;
    x[x >= 0.06 & x < 0.26] <- 3;
    x[x >= 0.01 & x < 0.06] <- 2;
    x[x < 0.01]             <- 1;
    classes[, c("ERU", "ERM", "ERL", "ELU", "ELM", "ELL")] <- x;
    classes[classes == 0] <- 1; # Both 0 and 1 means 0%

    ## We have two raters each giving 6 regions one of 6 possible labels 
    ## We treat each region separately, giving us 6 tables of 6x6 agreement classes
    ELL.L1 <- xtabs(formula = ~ ELL.L1 + ELL, data = classes);   
    ELM.L1 <- xtabs(formula = ~ ELM.L1 + ELM, data = classes);
    ELU.L1 <- xtabs(formula = ~ ELU.L1 + ELU, data = classes);
    ERL.L1 <- xtabs(formula = ~ ERL.L1 + ERL, data = classes);
    ERM.L1 <- xtabs(formula = ~ ERM.L1 + ERM, data = classes);
    ERU.L1 <- xtabs(formula = ~ ERU.L1 + ERU, data = classes);

    p0.L1 <- data.frame(list(ERU=sum(diag(ERU.L1))/sum(ERU.L1),
                             ERM=sum(diag(ERM.L1))/sum(ERM.L1),
                             ERL=sum(diag(ERL.L1))/sum(ERL.L1),
                             ELU=sum(diag(ELU.L1))/sum(ELM.L1),
                             ELM=sum(diag(ELU.L1))/sum(ELU.L1),
                             ELL=sum(diag(ELL.L1))/sum(ELL.L1)))

    ps.L1 <- data.frame(list(ERU=c(NaN,NaN,NaN,NaN,NaN,NaN),
                             ERM=c(NaN,NaN,NaN,NaN,NaN,NaN),
                             ERL=c(NaN,NaN,NaN,NaN,NaN,NaN),
                             ELU=c(NaN,NaN,NaN,NaN,NaN,NaN),
                             ELM=c(NaN,NaN,NaN,NaN,NaN,NaN),
                             ELL=c(NaN,NaN,NaN,NaN,NaN,NaN)))
    for ( i in seq(1,length(diag(ERU.L1))) ) {
        ps.L1[i, "ERU"] <- ERU.L1[i,i] / (sum(ERU.L1[i,]) + sum(ERU.L1[,i]));
    }
    for ( i in seq(1,length(diag(ERM.L1))) ) {
        ps.L1[i, "ERM"] <- ERM.L1[i,i] / (sum(ERM.L1[i,]) + sum(ERM.L1[,i]));
    }
    for ( i in seq(1,length(diag(ERL.L1))) ) {
        ps.L1[i, "ERL"] <- ERL.L1[i,i] / (sum(ERL.L1[i,]) + sum(ERL.L1[,i]));
    }
    for ( i in seq(1,length(diag(ELU.L1))) ) {
        ps.L1[i, "ELU"] <- ELU.L1[i,i] / (sum(ELU.L1[i,]) + sum(ELU.L1[,i]));
    }
    for ( i in seq(1,length(diag(ELM.L1))) ) {
        ps.L1[i, "ELM"] <- ELM.L1[i,i] / (sum(ELM.L1[i,]) + sum(ELM.L1[,i]));
    }
    for ( i in seq(1,length(diag(ELL.L1))) ) {
        ps.L1[i, "ELL"] <- ELL.L1[i,i] / (sum(ELL.L1[i,]) + sum(ELL.L1[,i]));
    }
    
    ELL.M1 <- xtabs(formula = ~ ELL.M1 + ELL, data = classes);   
    ELM.M1 <- xtabs(formula = ~ ELM.M1 + ELM, data = classes);
    ELU.M1 <- xtabs(formula = ~ ELU.M1 + ELU, data = classes);
    ERL.M1 <- xtabs(formula = ~ ERL.M1 + ERL, data = classes);
    ERM.M1 <- xtabs(formula = ~ ERM.M1 + ERM, data = classes);
    ERU.M1 <- xtabs(formula = ~ ERU.M1 + ERU, data = classes);
    p0.M1 <- data.frame(list(ERU=sum(diag(ERU.M1))/sum(ERU.M1),
                             ERM=sum(diag(ERM.M1))/sum(ERM.M1),
                             ERL=sum(diag(ERL.M1))/sum(ERL.M1),
                             ELU=sum(diag(ELU.M1))/sum(ELM.M1),
                             ELM=sum(diag(ELU.M1))/sum(ELU.M1),
                             ELL=sum(diag(ELL.M1))/sum(ELL.M1)))

    ps.M1 <- data.frame(list(ERU=c(NaN,NaN,NaN,NaN,NaN,NaN),
                             ERM=c(NaN,NaN,NaN,NaN,NaN,NaN),
                             ERL=c(NaN,NaN,NaN,NaN,NaN,NaN),
                             ELU=c(NaN,NaN,NaN,NaN,NaN,NaN),
                             ELM=c(NaN,NaN,NaN,NaN,NaN,NaN),
                             ELL=c(NaN,NaN,NaN,NaN,NaN,NaN)))
    for ( i in seq(1,6) ) {
        if ( i <= dim(ERU.M1)[1] && i <= dim(ERU.M1)[2] ) {
            ps.M1[i, "ERU"] <- ERU.M1[i,i] / (sum(ERU.M1[i,]) + sum(ERU.M1[,i]));
        }
        else if ( i <= dim(ERU.M1)[1] || i <= dim(ERU.M1)[2] ) {
            ps.M1[i, "ERU"] <- 0;
        }
        
        if ( i <= dim(ERM.M1)[1] && i <= dim(ERM.M1)[2] ) {
            ps.M1[i, "ERM"] <- ERM.M1[i,i] / (sum(ERM.M1[i,]) + sum(ERM.M1[,i]));
        }
        else if ( i <= dim(ERM.M1)[1] || i <= dim(ERM.M1)[2] ) {
            ps.M1[i, "ERM"] <- 0;
        }
        
        if ( i <= dim(ERL.M1)[1] && i <= dim(ERL.M1)[2] ) {
            ps.M1[i, "ERL"] <- ERL.M1[i,i] / (sum(ERL.M1[i,]) + sum(ERL.M1[,i]));
        }
        else if ( i <= dim(ERL.M1)[1] || i <= dim(ERL.M1)[2] ) {
            ps.M1[i, "ERL"] <- 0;
        }
        
        if ( i <= dim(ELU.M1)[1] && i <= dim(ELU.M1)[2] ) {
            ps.M1[i, "ELU"] <- ELU.M1[i,i] / (sum(ELU.M1[i,]) + sum(ELU.M1[,i]));
        }
        else if ( i <= dim(ELU.M1)[1] || i <= dim(ELU.M1)[2] ) {
            ps.M1[i, "ELU"] <- 0;
        }
        
        if ( i <= dim(ELM.M1)[1] && i <= dim(ELM.M1)[2] ) {
            ps.M1[i, "ELM"] <- ELM.M1[i,i] / (sum(ELM.M1[i,]) + sum(ELM.M1[,i]));
        }
        else if ( i <= dim(ELM.M1)[1] || i <= dim(ELM.M1)[2] ) {
            ps.M1[i, "ELM"] <- 0;
        }
        
        if ( i <= dim(ELL.M1)[1] && i <= dim(ELL.M1)[2] ) {
            ps.M1[i, "ELL"] <- ELL.M1[i,i] / (sum(ELL.M1[i,]) + sum(ELL.M1[,i]));
        }
        else if ( i <= dim(ELL.M1)[1] || i <= dim(ELL.M1)[2] ) {
            ps.M1[i, "ELL"] <- 0;
        }
    }
    
    ## for ( i in seq(1,length(diag(ERU.M1))) ) {
    ##     ps.M1[i, "ERU"] <- ERU.M1[i,i] / (sum(ERU.M1[i,]) + sum(ERU.M1[,i]));
    ##     ps.M1[i, "ERM"] <- ERM.M1[i,i] / (sum(ERM.M1[i,]) + sum(ERM.M1[,i]));
    ##     ps.M1[i, "ERL"] <- ERL.M1[i,i] / (sum(ERL.M1[i,]) + sum(ERL.M1[,i]));
    ##     ps.M1[i, "ELU"] <- ELU.M1[i,i] / (sum(ELU.M1[i,]) + sum(ELU.M1[,i]));
    ##     ps.M1[i, "ELM"] <- ELM.M1[i,i] / (sum(ELM.M1[i,]) + sum(ELM.M1[,i]));
    ##     ps.M1[i, "ELL"] <- ELL.M1[i,i] / (sum(ELL.M1[i,]) + sum(ELL.M1[,i]));
    ## }

   
  
    cat('\n========= Proportion of overall agreement ========\n')    
    cat('\t\tL1/CMS\n')
    print(round(p0.L1,2))
    cat('\n\t\tM1/CMS\n')
    print(round(p0.M1,2))

    cat('\n======== Proportion of specific agreement ========\n')    
    cat('\t\tL1/CMS\n')
    print( round(ps.L1,2) );
    cat('\n\t\tM1/CMS\n')
    print( round(ps.M1,2) );

    
    ## print( ELL.L1 ); print( ELL.M1 ); 
    ## print( ELM.L1 ); print( ELM.M1 );
    ## print( ELU.L1 ); print( ELU.M1 );
    ## print( ERL.L1 ); print( ERL.M1 );
    ## print( ERM.L1 ); print( ERM.M1 );
    ## print( ERU.L1 ); print( ERU.M1 );
    
    ## 3. We can measure the correlation between predictions and the reference
    


    
    ## x <- predictions[, c("ERU", "ERM", "ERL", "ELU", "ELM", "ELL")];
    ## x[x >= 0.76]            <- 6;
    ## x[x >= 0.51 & x < 0.76] <- 5;
    ## x[x >= 0.26 & x < 0.51] <- 4;
    ## x[x >= 0.06 & x < 0.26] <- 3;
    ## x[x >= 0.01 & x < 0.06] <- 2;
    ## x[x < 0.01] <- 1;
    ## predictions[, c("ERU", "ERM", "ERL", "ELU", "ELM", "ELL")] <- x;
        
    ## scores <- merge(visualScores, predictions, by='IdDate');
    ## print( scores );

    ## We have two raters each giving 6 regions one of 6 possible labels 
    ## We treat each region separately, giving us 6 tables of 6x6 agreement scores
    ## ELL.L1 <- xtabs(formula = ~ ELL.L1 + ELL, data = scores);
    ## ELL.M1 <- xtabs(formula = ~ ELL.M1 + ELL, data = scores);
    ## ELM.L1 <- xtabs(formula = ~ ELM.L1 + ELM, data = scores);
    ## ELM.M1 <- xtabs(formula = ~ ELM.M1 + ELM, data = scores);
    ## ELU.L1 <- xtabs(formula = ~ ELU.L1 + ELU, data = scores);
    ## ELU.M1 <- xtabs(formula = ~ ELU.M1 + ELU, data = scores);
    ## ERL.L1 <- xtabs(formula = ~ ERL.L1 + ERL, data = scores);
    ## ERL.M1 <- xtabs(formula = ~ ERL.M1 + ERL, data = scores);
    ## ERM.L1 <- xtabs(formula = ~ ERM.L1 + ERM, data = scores);
    ## ERM.M1 <- xtabs(formula = ~ ERM.M1 + ERM, data = scores);
    ## ERU.L1 <- xtabs(formula = ~ ERU.L1 + ERU, data = scores);
    ## ERU.M1 <- xtabs(formula = ~ ERU.M1 + ERU, data = scores);

    ## print( ELL.L1 ); print( ELL.M1 ); print( cor(ELL.L1) );
    ## print( ELM.L1 ); print( ELM.M1 );
    ## print( ELU.L1 ); print( ELU.M1 );
    ## print( ERL.L1 ); print( ERL.M1 );
    ## print( ERM.L1 ); print( ERM.M1 );
    ## print( ERU.L1 ); print( ERU.M1 );
}


options <- commandArgs(trailingOnly=TRUE);
main(options);
