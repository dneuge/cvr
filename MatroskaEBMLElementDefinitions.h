#ifndef MATROSKAEBMLELEMENTDEFINITIONS_H
#define	MATROSKAEBMLELEMENTDEFINITIONS_H

#include <string>
#include <map>

#include "EBMLElement.h"

/**
 * Contains all Matroska EBML element definitions defined in original specifications.
 * Instance should be kept as long as EBML tree exists as destructor will free all
 * definitions which were retrieved as shared pointers.
 * @see http://www.matroska.org/technical/specs/index.html
 */
class MatroskaEBMLElementDefinitions {
    public:
        MatroskaEBMLElementDefinitions();
        ~MatroskaEBMLElementDefinitions();
        EBMLElement* getElementDefinitionByName(char*);
    
    private:
        std::map<std::string, EBMLElement*> elementDefinitions;
};

#endif	/* MATROSKAEBMLELEMENTDEFINITIONS_H */
