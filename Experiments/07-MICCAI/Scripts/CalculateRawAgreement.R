#!/usr/bin/Rscript

main <- function(options) {
    if ( length(options) < 2 ) {
        print( "Usage: <visual-scores> <predictions> [<t>]" );
        quit( status = 1 );
    }

    t <- 0
    if ( length(options) >= 3 ) {
        t <- options[3]
    }
        
    CalculateRawAgreement( options[1], options[2], t );
    quit( status = 0 );
}

CalculateRawAgreement <- function(visualScoresPath, predictionsPath, t) {
    library(irr)
    visualScores <- read.csv(visualScoresPath, header=TRUE);
    predictions <- read.csv(predictionsPath, header=TRUE);

    classes <- merge(visualScores, predictions, by='IdDate');       
    regions <- c("ERU", "ERM", "ERL", "ELU", "ELM", "ELL")
    
    pdf(file='test.pdf')
    plot(classes$ERU.L1 , classes$ERU);
    dev.off();

    for ( region in regions ) {
        CMS <- classes[,region]
        L1 <- classes[,paste(region, 'L1', sep=".")]
        L1.p <- sum(L1 > 1)
        L1.n <- sum(L1 <= 1)
        M1 <- classes[,paste(region, 'M1', sep=".")]
        M1.p <- sum(M1 > 1)
        M1.n <- sum(M1 <= 1)
        cat( region )
        con.tab <- array(data=c(sum(L1 >  1 & CMS >  t )/L1.p,  
                             sum(   L1 >  1 & CMS <= t )/L1.p,
                             sum(   L1 <= 1 & CMS >  t )/L1.n,
                             sum(   L1 <= 1 & CMS <= t )/L1.n,
                             sum(   M1 >  1 & CMS >  t )/M1.p,  
                             sum(   M1 >  1 & CMS <= t )/M1.p,
                             sum(   M1 <= 1 & CMS >  t )/M1.n,
                             sum(   M1 <= 1 & CMS <= t )/M1.n),
                         dim=c(2,4),
                         dimnames=list(
                             CMS=c('Presence','Absence'),
                             Rater=c('L1.Presence','L1.Absence', 'M1.Presence','M1.Absence')))
        print(round(con.tab,2));
        cat( '\n' )

        ## CMS <- classes[,region]
        ## L1 <- classes[,paste(region, 'L1', sep=".")]
        ## p <- sum(L1 > 1)
        ## n <- sum(L1 <= 1)
        ## con.tab <- array(data=c(sum(L1 >  1 & CMS >  0 )/p,  
        ##                      sum(   L1 >  1 & CMS <= 0 )/p,
        ##                      sum(   L1 <= 1 & CMS >  0 )/n,
        ##                      sum(   L1 <= 1 & CMS <= 0 )/n),
        ##                  dim=c(2,2),
        ##                  dimnames=list(CMS=c('Presence','Absence'),L1=c('Presence','Absence')))
        ## print(round(con.tab,2));

        ## M1 <- classes[,paste(region, 'M1', sep=".")]
        ## p <- sum(M1 > 1)
        ## n <- sum(M1 <= 1)
        ## con.tab <- array(data=c(sum(M1 >  1 & CMS >  0 )/p,  
        ##                      sum(   M1 >  1 & CMS <= 0 )/p,
        ##                      sum(   M1 <= 1 & CMS >  0 )/n,
        ##                      sum(   M1 <= 1 & CMS <= 0 )/n),
        ##                  dim=c(2,2),
        ##                  dimnames=list(CMS=c('Presence','Absence'),M1=c('Presence','Absence')))
        ## print(round(con.tab,2));
    }
    
    

    ## 3. We can look at contingency tables
    x <- round(classes[, regions], 2);
    x[x >= 0.76]            <- 6;
    x[x >= 0.51 & x < 0.76] <- 5;
    x[x >= 0.26 & x < 0.51] <- 4;
    x[x >= 0.06 & x < 0.26] <- 3;
    x[x >= 0.01 & x < 0.06] <- 2;
    x[x < 0.01]             <- 1;
    classes[, regions] <- x;
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
    for ( i in seq(1,6) ) {
        if ( i <= dim(ERU.L1)[1] && i <= dim(ERU.L1)[2] ) {
            ps.L1[i, "ERU"] <- 2*ERU.L1[i,i] / (sum(ERU.L1[i,]) + sum(ERU.L1[,i]));
        }
        else if ( i <= dim(ERU.L1)[1] || i <= dim(ERU.L1)[2] ) {
            ps.L1[i, "ERU"] <- 0;
        }
        
        if ( i <= dim(ERM.L1)[1] && i <= dim(ERM.L1)[2] ) {
            ps.L1[i, "ERM"] <- 2*ERM.L1[i,i] / (sum(ERM.L1[i,]) + sum(ERM.L1[,i]));
        }
        else if ( i <= dim(ERM.L1)[1] || i <= dim(ERM.L1)[2] ) {
            ps.L1[i, "ERM"] <- 0;
        }
        
        if ( i <= dim(ERL.L1)[1] && i <= dim(ERL.L1)[2] ) {
            ps.L1[i, "ERL"] <- 2*ERL.L1[i,i] / (sum(ERL.L1[i,]) + sum(ERL.L1[,i]));
        }
        else if ( i <= dim(ERL.L1)[1] || i <= dim(ERL.L1)[2] ) {
            ps.L1[i, "ERL"] <- 0;
        }
        
        if ( i <= dim(ELU.L1)[1] && i <= dim(ELU.L1)[2] ) {
            ps.L1[i, "ELU"] <- 2*ELU.L1[i,i] / (sum(ELU.L1[i,]) + sum(ELU.L1[,i]));
        }
        else if ( i <= dim(ELU.L1)[1] || i <= dim(ELU.L1)[2] ) {
            ps.L1[i, "ELU"] <- 0;
        }
        
        if ( i <= dim(ELM.L1)[1] && i <= dim(ELM.L1)[2] ) {
            ps.L1[i, "ELM"] <- 2*ELM.L1[i,i] / (sum(ELM.L1[i,]) + sum(ELM.L1[,i]));
        }
        else if ( i <= dim(ELM.L1)[1] || i <= dim(ELM.L1)[2] ) {
            ps.L1[i, "ELM"] <- 0;
        }
        
        if ( i <= dim(ELL.L1)[1] && i <= dim(ELL.L1)[2] ) {
            ps.L1[i, "ELL"] <- 2*ELL.L1[i,i] / (sum(ELL.L1[i,]) + sum(ELL.L1[,i]));
        }
        else if ( i <= dim(ELL.L1)[1] || i <= dim(ELL.L1)[2] ) {
            ps.L1[i, "ELL"] <- 0;
        }
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
            ps.M1[i, "ERU"] <- 2*ERU.M1[i,i] / (sum(ERU.M1[i,]) + sum(ERU.M1[,i]));
        }
        else if ( i <= dim(ERU.M1)[1] || i <= dim(ERU.M1)[2] ) {
            ps.M1[i, "ERU"] <- 0;
        }
        
        if ( i <= dim(ERM.M1)[1] && i <= dim(ERM.M1)[2] ) {
            ps.M1[i, "ERM"] <- 2*ERM.M1[i,i] / (sum(ERM.M1[i,]) + sum(ERM.M1[,i]));
        }
        else if ( i <= dim(ERM.M1)[1] || i <= dim(ERM.M1)[2] ) {
            ps.M1[i, "ERM"] <- 0;
        }
        
        if ( i <= dim(ERL.M1)[1] && i <= dim(ERL.M1)[2] ) {
            ps.M1[i, "ERL"] <- 2*ERL.M1[i,i] / (sum(ERL.M1[i,]) + sum(ERL.M1[,i]));
        }
        else if ( i <= dim(ERL.M1)[1] || i <= dim(ERL.M1)[2] ) {
            ps.M1[i, "ERL"] <- 0;
        }
        
        if ( i <= dim(ELU.M1)[1] && i <= dim(ELU.M1)[2] ) {
            ps.M1[i, "ELU"] <- 2*ELU.M1[i,i] / (sum(ELU.M1[i,]) + sum(ELU.M1[,i]));
        }
        else if ( i <= dim(ELU.M1)[1] || i <= dim(ELU.M1)[2] ) {
            ps.M1[i, "ELU"] <- 0;
        }
        
        if ( i <= dim(ELM.M1)[1] && i <= dim(ELM.M1)[2] ) {
            ps.M1[i, "ELM"] <- 2*ELM.M1[i,i] / (sum(ELM.M1[i,]) + sum(ELM.M1[,i]));
        }
        else if ( i <= dim(ELM.M1)[1] || i <= dim(ELM.M1)[2] ) {
            ps.M1[i, "ELM"] <- 0;
        }
        
        if ( i <= dim(ELL.M1)[1] && i <= dim(ELL.M1)[2] ) {
            ps.M1[i, "ELL"] <- 2*ELL.M1[i,i] / (sum(ELL.M1[i,]) + sum(ELL.M1[,i]));
        }
        else if ( i <= dim(ELL.M1)[1] || i <= dim(ELL.M1)[2] ) {
            ps.M1[i, "ELL"] <- 0;
        }
    }   
  
    ## cat('\n========= Proportion of overall agreement ========\n')    
    ## cat('\t\tL1/CMS\n')
    ## print(round(p0.L1,2))
    ## cat('\n\t\tM1/CMS\n')
    ## print(round(p0.M1,2))

    ## cat('\n======== Proportion of specific agreement ========\n')    
    ## cat('\t\tL1/CMS\n')
    ## print( round(ps.L1,2) );
    ## cat('\n\t\tM1/CMS\n')
    ## print( round(ps.M1,2) );    
}


options <- commandArgs(trailingOnly=TRUE);
main(options);
