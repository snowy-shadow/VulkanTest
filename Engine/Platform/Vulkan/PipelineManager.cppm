module;
#include <unordered_map>
#include <string>

export module VT.Platform.Vulkan.PipelineManager;

import VT.Pipeline;

export namespace VT::Vulkan
{
	class PiplineManager
	{
    public:
        void Add(VT::Pipeline* Pipeline);
    private:
		std::unordered_map<std::string, VT::Pipeline> m_Pipeline;
	};
    void PiplineManager::Add(VT::Pipeline* Pipeline, std::string Name)
    {
        m_Pipeline.insert(std::move(Name), Pipeline);
    }
    } // namespace VT::Vulkan
}