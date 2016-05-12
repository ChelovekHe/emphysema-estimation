#ifndef __UncertainLabel_h
#define __UncertainLabel_h

/**
   \brief Label indicating label value and label uncertainty, e.g ('a', 0.5)
 */
template< typename TLabel, typename TUncertainty >
struct UncertainLabel {
  typedef TLabel ValueType;
  typedef TUncertainty UncertaintyType;

  ValueType label;
  UncertaintyType uncertainty;
}
  

#endif
