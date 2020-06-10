/*
 * AbstractParamPresentation.h
 *
 * Copyright (C) 2020 by VISUS (Universitaet Stuttgart).
 * Alle Rechte vorbehalten.
 */

#ifndef MEGAMOLCORE_ABSTRACTPARAMPRESENTATION_H_INCLUDED
#define MEGAMOLCORE_ABSTRACTPARAMPRESENTATION_H_INCLUDED

#include "mmcore/api/MegaMolCore.std.h"

#include "vislib/sys/Log.h"


namespace megamol {
namespace core {
namespace param {


class MEGAMOLCORE_API AbstractParamPresentation {
public:

    // Available parameter types
    enum ParamType {
        BOOL,
        BUTTON,
        COLOR,
        ENUM,
        FILEPATH,
        FLEXENUM,
        FLOAT,
        INT,
        STRING,
        TERNARY,
        TRANSFERFUNCTION,
        VECTOR2F,
        VECTOR3F,
        VECTOR4F,
        UNKNOWN
    };

    enum Presentation : int {  /// (limited to 32 options)
        NONE                = 0,
        Basic               = 1 << 1,       // Basic widget (is supported for all parameter types) -> Default
        String              = 1 << 2,       // String widget (is supported for all types, uses ValueString function of parameters)
        Color               = 1 << 3,       // Color editor widget
        FilePath            = 1 << 4,       // File path widget
        TransferFunction    = 1 << 5,       // Transfer function editor widget
        PinValueToMouse     = 1 << 6        // Pin parameter value to mouse position
    };

    /**
    * Initalise presentation for parameter once.
    *
    * @param param_type   The parameters type.
    */
    bool InitPresentation(ParamType param_type);

    /**
    * Answer visibility in GUI.
    *
    * @return GUI visibility
    */
    inline bool IsGUIVisible(void) const {
        return this->visible;
    }

    /**
    * Set visibility in GUI.
    *
    * @param visible True: visible in GUI, false: invisible
    */
    inline void SetGUIVisible(bool visible) {
        this->visible = visible;
    }

    /**
    * Answer accessibility in GUI.
    *
    * @return GUI accessibility
    */
    inline bool IsGUIReadOnly(void) const {
        return this->read_only;
    }

    /**
    * Set accessibility in GUI.
    *
    * @param read_only True: read-only in GUI, false: writable
    */
    inline void SetGUIReadOnly(bool read_only) {
        this->read_only = read_only;
    }     
        
    /**
    * Set presentation of parameter in GUI.
    *
    * @param presentation Presentation of parameter in GUI.
    *
    * @return True if given presentation is compatible, false otherwise.
    */
    void SetGUIPresentation(Presentation presentation);

    /**
    * Answer parameter presentation in GUI.
    *
    * @return Parameter presentation.
    */
    inline Presentation GetGUIPresentation(void) const {
        return this->presentation;
    }

    /**
    * Answer whether given presentation is compatible with parameter.
    *
    * @return True if given presentation is compatible, false otherwise.
    */
    inline bool IsPresentationCompatible(Presentation presentation) const {
        return (Presentation::NONE != (presentation & this->compatible));
    }

protected:

    AbstractParamPresentation(void);

    virtual ~AbstractParamPresentation(void) = default;

private:

    // VARIABLES --------------------------------------------------------------

    /* Show or hide the parameter in the GUI.
       Paramter is implicitly hidden in GUI if other than raw value view is selected. */
    bool visible;

    /* Make parameter read-only in the GUI. */
    bool read_only;
    
    /* Presentation (= widget representation) of parameter in the GUI. */
    Presentation presentation;

    /* Compatible presentations */
    Presentation compatible;

    /* Falg ensuring that initialisation can only be applied once. */
    bool initialised;
};


inline AbstractParamPresentation::Presentation operator|(AbstractParamPresentation::Presentation a, AbstractParamPresentation::Presentation b) {
    return static_cast<AbstractParamPresentation::Presentation>(static_cast<int>(a) | static_cast<int>(b));
}
inline AbstractParamPresentation::Presentation operator&(AbstractParamPresentation::Presentation a, AbstractParamPresentation::Presentation b) {
    return static_cast<AbstractParamPresentation::Presentation>(static_cast<int>(a) & static_cast<int>(b));
}


} /* end namespace param */
} /* end namespace core */
} /* end namespace megamol */

#endif /* MEGAMOLCORE_ABSTRACTPARAMPRESENTATION_H_INCLUDED */
