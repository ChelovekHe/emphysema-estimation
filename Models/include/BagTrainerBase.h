#ifndef __BagTrainerBase_h
#define __BagTrainerBase_h


#include "BaggedDataset.h"

template< typename TModelTrainer, typename TTracer >
class BagTrainerBase : private TModelTrainer, private TTracer {
public: 

  /**
     \brief Start model training. Training can be paused/resumed
     
     \param model  The model to train
     \patam bags   Training data as a collection of bags
   */
  void Start( ModelType& model, const BaggedDataset& bags) {
    // Set signal handler
    // Call Train asynch
    // Pause
    // Train( model, bags );
  }

  
 
  /**
     \brief Pause training as soon as possible.
            Must leave model/training in a resumeable state.

     \sa ::Resume
   */
  void Pause();

  /**
     \brief Resume previously paused training.

     \sa ::Pause
   */
   void Resume();


  
  

  /**
     \brief Toggle if training is traced

     \param traceOn  If true then a trace of the training is generated
   */
  virtual void ToggleTrace(bool traceOn) {
    m_TraceOn = traceOn;
  }

  /**
     \brief Get the trace of training

  */
  const Trace& Trace() {
    return m_Trace;
  }

protected:
  TraceType m_Trace;
  bool m_TraceOn;
  
};
