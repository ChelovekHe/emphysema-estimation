#ifndef __InstanceBase_h
#define __InstanceBase_h

template< typename TFeature, typename TLabel >
class InstanceBase {
public:
  typedef TFeature FeatureType;
  typedef TLabel LabelType;
  typedef InstanceBase< FeatureType, LabelType > Self;  

  /**
     \brief Construct an instance from a feature vector and a label

     \param features  A feature vector
     \param label     A label
  */
  InstanceBase( FeatureType& features, LabelType& label )
    : m_Features( features ),
      m_Label( labels )
  { }

  /**
     \brief Return the feature vector of the instance
  */
  FeatureType& Features() {
    return m_Features;
  }

  /**
     \brief Get the dimension of the feature space
  */
  std::size_t Dimension() {
    return m_Features.size();
  }
  
  /**
     \brief Return the label associated with the instance
  */
  LabelType& Label() {
    return m_Label;
  }
  
protected:
  FeatureType m_Features;
  LabelType m_Label;
  
};

#endif
