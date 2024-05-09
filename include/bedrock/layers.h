#ifndef BEDROCK_LAYERS_H
#define BEDROCK_LAYERS_H

#include <set>
#include <map>

namespace Bedrock{
    class LayerManager{
    private:
        std::map<LayerId, std::set<ClientID>> layerMap; //TODO
        std::map<ClientID, LayerId> idToLayerMap; //TODO
    public:
        static LayerManager& getInstance(){
            static LayerManager instance;
            return instance;
        }

        void addClientToLayer(const ClientID& client, const LayerId& layer);
        void removeClientFromLayers(const ClientID& client);
        std::set<ClientID> getIdsFromLayer(const LayerId& layer);
    };


    void addClientToLayer(const ClientID& client, const LayerId& layer);
    void removeClientFromLayers(const ClientID& client);
    std::set<ClientID> getIdsFromLayer(const LayerId& layer);
}


#endif //BEDROCK_LAYERS_H
