#include "ElasticityTensorAssembler.h"

#include <Assembler/Materials/Material.h>
#include <Assembler/Math/MatrixOrder.h>

ZSparseMatrix ElasticityTensorAssembler::assemble(FESettingPtr setting) {
    typedef FESetting::FEMeshPtr FEMeshPtr;
    typedef FESetting::MaterialPtr MaterialPtr;

    typedef Eigen::Triplet<Float> T;
    std::vector<T> entries;

    FEMeshPtr mesh = setting->get_mesh();
    MaterialPtr material = setting->get_material();

    const size_t dim = mesh->getDim();
    const size_t num_elements = mesh->getNbrElements();
    MatrixI order = MatrixOrder::get_order(dim);
    size_t num_entries_per_element = dim * (dim+1) / 2;

    for (size_t i=0; i<num_elements; i++) {
        size_t base = i * num_entries_per_element;
        for (size_t j=0; j<dim; j++) {
            for (size_t k=j; k<dim; k++) {
                size_t tensor_row = order(j, k);
                for (size_t m=0; m<dim; m++) {
                    for (size_t n=0; n<dim; n++) {
                        size_t tensor_col = order(m, n);
                        entries.push_back(T(
                                    base + tensor_row, base + tensor_col,
                                    material->get_material_tensor(j, k, m, n)));
                    }
                }
            }
        }
    }

    ZSparseMatrix C(num_elements * num_entries_per_element,
            num_elements* num_entries_per_element);
    C.setFromTriplets(entries.begin(), entries.end());
    return C;
}
