/*
 * CalleeSlot.cpp
 *
 * Copyright (C) 2008 by Universitaet Stuttgart (VIS).
 * Alle Rechte vorbehalten.
 */

#include "mmcore/CalleeSlot.h"
#include "mmcore/AbstractNamedObject.h"
#include "mmcore/AbstractNamedObjectContainer.h"
#include "mmcore/CallerSlot.h"
#include "vislib/sys/AutoLock.h"

using namespace megamol::core;

/****************************************************************************/

/*
 * CalleeSlot::CalleeSlot
 */
CalleeSlot::CalleeSlot(const vislib::StringA& name, const vislib::StringA& desc)
        : AbstractSlot(name, desc)
        , AbstractCallSlotPresentation()
        , callbacks() {}


/*
 * CalleeSlot::~CalleeSlot
 */
CalleeSlot::~CalleeSlot() {
    for (unsigned int i = 0; i < this->callbacks.Count(); i++) {
        delete this->callbacks[i];
    }
    this->callbacks.Clear();
}


/*
 * CalleeSlot::ConnectCall
 */
bool CalleeSlot::ConnectCall(megamol::core::Call* call, factories::CallDescription::ptr call_description) {
    vislib::sys::AutoLock lock(this->Parent()->ModuleGraphLock());
    if (call == NULL) {
        this->SetStatusDisconnected(); // TODO: This is wrong! Reference counting!
        return true;
    }

    factories::CallDescriptionManager::description_ptr_type desc;
    // for the new MegaMolGraph, we don't want to handle ConreInstances.
    // so now we pass the call_description, which the graph holds anyway, to satisfy the code filling call->funcMap[]
    if (call_description == nullptr) {
        // This case was legacy support for the old mmconsole. It should now never be called. Keep this is as safety
        // check, because the old function signature is still in use in the Call destructor, but that should trigger
        // the above call == nullptr case.
        throw std::runtime_error("CalleeSlot::ConnectCall() - This case should not happen!");
    } else {
        desc = call_description;
    }

    vislib::StringA cn(desc->ClassName());
    for (unsigned int i = 0; i < desc->FunctionCount(); i++) {
        vislib::StringA fn(desc->FunctionName(i));
        for (unsigned int j = 0; j < this->callbacks.Count(); j++) {
            if (cn.Equals(this->callbacks[j]->CallName(), false) && fn.Equals(this->callbacks[j]->FuncName(), false)) {
                call->funcMap[i] = j;
                break;
            }
        }
    }
    call->callee = this;
    this->SetStatusConnected();
    return true;
}


/*
 * CalleeSlot::InCall
 */
bool CalleeSlot::InCall(unsigned int func, Call& call) {
    if (func >= this->callbacks.Count())
        return false;
    return this->callbacks[func]->CallMe(const_cast<Module*>(reinterpret_cast<const Module*>(this->Owner())), call);
}


/*
 * CalleeSlot::ClearCleanupMark
 */
void CalleeSlot::ClearCleanupMark() {
    if (!this->CleanupMark())
        return;

    AbstractNamedObject::ClearCleanupMark();
    if (this->Parent() != NULL) {
        this->Parent()->ClearCleanupMark();
    }
}
