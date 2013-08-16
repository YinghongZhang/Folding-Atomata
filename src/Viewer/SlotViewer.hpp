
#ifndef SLOT_VIEWER_HPP
#define SLOT_VIEWER_HPP

#include "../Trajectory/Trajectory.hpp"
#include "../Modeling/Mesh/Mesh.hpp"
#include "../World/Scene.hpp"

/*
SlotViewer handles the viewing of the protein from a particular slot.
The protein is displayed as soon as the topology comes in, and then explodes
into position once the first checkpoint becomes available. From there, it iterates
through all checkpoints, and then jumps to the first checkpoint and repeats.
If Tracers are enabled, it shows them.
*/
class SlotViewer
{
    public:
        SlotViewer(const TrajectoryPtr& trajectory, 
                   const std::shared_ptr<Scene>& scene
        );
        ~SlotViewer();
        void update();

    private:
        std::shared_ptr<Mesh> generateAtomMesh();
        std::shared_ptr<Mesh> generateBondMesh();

        void addAtomsToScene();
        void addBondsToScene();

        void addAtom(const AtomPtr& atom, const ModelPtr& model);
        void addBond(const BondPtr& bond, const ModelPtr& model);

    private:
        TrajectoryPtr trajectory_;
        std::shared_ptr<Scene> scene_;
        std::vector<ModelPtr> atomModels_;
        std::vector<ModelPtr> bondModels_;
};

#endif
