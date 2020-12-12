#include "InjectClusterID.h"

#include "ProbeCalls.h"

megamol::probe::InjectClusterID::InjectClusterID(void) : _version(0), _probes_rhs_slot("probes", ""), _mesh_rhs_slot("mesh",""), _mesh_lhs_slot("output_mesh","") {
    this->_probes_rhs_slot.SetCompatibleCall<megamol::probe::CallProbesDescription>();
    this->MakeSlotAvailable(&this->_probes_rhs_slot);

    this->_mesh_rhs_slot.SetCompatibleCall<megamol::mesh::CallMeshDescription>();
    this->MakeSlotAvailable(&this->_mesh_rhs_slot);

    this->_mesh_lhs_slot.SetCallback(
        mesh::CallMesh::ClassName(), mesh::CallMesh::FunctionName(0), &InjectClusterID::getData);
    this->_mesh_lhs_slot.SetCallback(
        mesh::CallMesh::ClassName(), mesh::CallMesh::FunctionName(1), &InjectClusterID::getMetaData);
    this->MakeSlotAvailable(&this->_mesh_lhs_slot);
}

megamol::probe::InjectClusterID::~InjectClusterID(void) {
    this->Release();
}

bool megamol::probe::InjectClusterID::create() {
    return true;
}

void megamol::probe::InjectClusterID::release() {}

bool megamol::probe::InjectClusterID::getMetaData(core::Call& call) {
    auto cm_lhs = dynamic_cast<mesh::CallMesh*>(&call);
    if (cm_lhs == nullptr) {
        return false;
    }
    auto meta_data_lhs = cm_lhs->getMetaData();

    mesh::CallMesh* cm_rhs = this->_mesh_rhs_slot.CallAs<mesh::CallMesh>();
    if (cm_rhs != nullptr) {
        auto meta_data_rhs = cm_rhs->getMetaData();
        meta_data_rhs.m_frame_ID = meta_data_lhs.m_frame_ID;
        cm_rhs->setMetaData(meta_data_rhs);
        if (!(*cm_rhs)(1)) {
            return false;
        }
        meta_data_rhs = cm_rhs->getMetaData();
        meta_data_lhs = meta_data_rhs;
        cm_lhs->setMetaData(meta_data_lhs);
    }

    return true;
}

bool megamol::probe::InjectClusterID::getData(core::Call& call) {
    auto cm_lhs = dynamic_cast<mesh::CallMesh*>(&call);
    if (cm_lhs == nullptr) {
        return false;
    }

    std::shared_ptr<mesh::MeshDataAccessCollection> mesh_collection;

    mesh::CallMesh* cm_rhs = this->_mesh_rhs_slot.CallAs<mesh::CallMesh>();
    probe::CallProbes* cp_rhs = this->_probes_rhs_slot.CallAs<probe::CallProbes>();
    if (cm_rhs != nullptr && cp_rhs != nullptr) {

        auto something_has_changed = cm_rhs->hasUpdate() || cp_rhs->hasUpdate();

        if (something_has_changed) {

            if (!(*cm_rhs)(0) || !(*cp_rhs)(0)) {
                return false;
            }

            ++_version;
            auto meshes = cm_rhs->getData();
            auto probes = cp_rhs->getData();

            for (auto& mesh : meshes->accessMeshes()) {
                for (auto& attribute : mesh.second.attributes) {
                    if (attribute.semantic == mesh::MeshDataAccessCollection::ID) {
                        auto probe_id_accessor = reinterpret_cast<uint32_t*>(attribute.data);
                        auto vertex_cnt = attribute.byte_size / sizeof(uint32_t);

                        for (int i = 0; i < vertex_cnt; ++i) {
                            auto generic_probe = probes->getGenericProbe(probe_id_accessor[i]);

                            uint32_t cluster_id = std::visit([](auto&& arg) -> uint32_t { return arg.m_cluster_id; }, generic_probe);

                            probe_id_accessor[i] = cluster_id;
                        }
                    }
                }
            }
        }

        auto meshes = cm_rhs->getData();
        mesh_collection = meshes;
    }

    cm_lhs->setData(mesh_collection, _version);

    return true;
}
