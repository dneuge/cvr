#ifndef EBMLELEMENT_H
#define	EBMLELEMENT_H

class EBMLElement {
    public:
        EBMLElement(unsigned char, const unsigned char*);
        ~EBMLElement();
        unsigned char* getSequence();
        unsigned char getSequenceLength();
        
    private:
        unsigned char *sequence;
        unsigned char sequenceLength;
};

#endif	/* EBMLELEMENT_H */

