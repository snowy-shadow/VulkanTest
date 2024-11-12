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
        bool Add(VT::Pipeline* Pipeline, std::string Name);
    private:
		std::unordered_map<std::string, VT::Pipeline*> m_Pipeline;
	};

    bool PiplineManager::Add(VT::Pipeline* Pipeline, std::string Name)
    {
       return m_Pipeline.insert({std::move(Name), Pipeline}).second;
    }
} // namespace VT::Vulkan
