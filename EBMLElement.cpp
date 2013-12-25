#include "EBMLElement.h"

/**
 * Creates a new EBML element definition.
 * @param sequenceLength length of sequence code
 * @param sequence sequence code (shared pointer, do not manipulate/free after use, will be freed on destruction!)
 */
EBMLElement::EBMLElement(unsigned char sequenceLength, const unsigned char *sequence) {
    this->sequence = (unsigned char*) sequence;
    this->sequenceLength = sequenceLength;
}

EBMLElement::~EBMLElement() {
    delete sequence;
}

/**
 * Returns shared pointer to element sequence code.
 * @return shared pointer to sequence code (do not free after use!)
 */
unsigned char* EBMLElement::getSequence() {
    return sequence;
}

/**
 * Returns length of element sequence code.
 * @return length of sequence code
 */
unsigned char EBMLElement::getSequenceLength() {
    return sequenceLength;
}
