#ifndef __CrossValidatorParams_h
#define __CrossValidatorParams_h

enum class CrossValidationType: char { LEAVE_P_OUT, K_FOLD };

std::ostream& operator<<(std::ostream& out, CrossValidationType cvt ) {
  switch ( cvt ) {
  case CrossValidationType::LEAVE_P_OUT:
    out << "Leave-p-out";
    break;

  case CrossValidationType::K_FOLD:
    out << "k-Fold";
    break;

  }
  return out;
}

struct CrossValidationParams {
  CrossValidationParams( CrossValidationType cvType,
			 unsigned int k,
			 bool shuffle )
    : cvType( cvType ),
      k( k ),
      shuffle( shuffle )
  {}

  CrossValidationType cvType; // The kind of CV to perform
  union {
    unsigned int p; // For leave-p-out CV
    unsigned int k; // For k-fold CV
  };
  bool shuffle; // Should the cross validator shuffle the data before
                // partitioning.
};

#endif
