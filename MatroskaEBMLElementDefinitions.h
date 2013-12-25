#ifndef MATROSKAEBMLELEMENTDEFINITIONS_H
#define	MATROSKAEBMLELEMENTDEFINITIONS_H

#include <string>
#include <map>

#include "EBMLElement.h"

#define MATROSKA_TRACK_TYPE_VIDEO 0x01
#define MATROSKA_TRACK_TYPE_AUDIO 0x02

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
        EBMLElement* getElementDefinitionByName(const char*);
    
    private:
        std::map<std::string, EBMLElement*> elementDefinitions;
};

#endif	/* MATROSKAEBMLELEMENTDEFINITIONS_H */
