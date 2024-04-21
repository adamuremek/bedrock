#include <bedrock/bedrock.h>

void Bedrock::LayerManager::addClientToLayer(const Bedrock::ClientID& client, const Bedrock::LayerId& layer) {
    auto it = idToLayerMap.find(client);
    if (it != idToLayerMap.end()) {
        LayerId currentLayer = it->second;
        if (currentLayer != layer) {
            removeClientFromLayers(client);
        }
    }
    layerMap[layer].insert(client);
    idToLayerMap[client] = layer;
}

void Bedrock::LayerManager::removeClientFromLayers(const Bedrock::ClientID& client) {
    auto it = idToLayerMap.find(client);
    if (it != idToLayerMap.end()) {
        LayerId currentLayer = it->second;
        layerMap[currentLayer].erase(client);
        if (layerMap[currentLayer].empty()) {
            layerMap.erase(currentLayer);
        }
        idToLayerMap.erase(client);
    }
}

std::set<Bedrock::ClientID> Bedrock::LayerManager::getIdsFromLayer(const Bedrock::LayerId& layer) {
    if (layerMap.find(layer) != layerMap.end()) {
        return layerMap[layer];
    }
    return {};  // Return an empty set if the layer does not exist
}

void Bedrock::addClientToLayer(const Bedrock::ClientID& client, const Bedrock::LayerId& layer) {
    LayerManager::getInstance().addClientToLayer(client, layer);
}

void Bedrock::removeClientFromLayers(const Bedrock::ClientID& client) {
    LayerManager::getInstance().removeClientFromLayers(client);
}

std::set<Bedrock::ClientID> Bedrock::getIdsFromLayer(const Bedrock::LayerId& layer) {
    return LayerManager::getInstance().getIdsFromLayer(layer);
}