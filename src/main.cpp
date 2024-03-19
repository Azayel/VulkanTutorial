#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


#include <cstddef>
#include <unordered_set>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

//Validation Layers
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};


#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

class HelloTriangleApplication {
public:
  void run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

private:
  // Attributes
  GLFWwindow *window;
  VkInstance instance;

  void initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
  }
  void initVulkan() {
    createInstance();
  }

  void createInstance(){
    // Application Info structure 
    VkApplicationInfo appInfo{};
    
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = NULL;
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_MAKE_VERSION(1,0,0);


    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensionNames;
    glfwExtensionNames = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    
    
    checkInstanceExtensionSupport(glfwExtensionNames, glfwExtensionCount);
    checkVailidationLayerSupoort();
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensionNames;
    createInfo.enabledLayerCount = 0;
    
   
    if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS){
      throw std::runtime_error{"Failed to Create Vulkan Instance!"};
    }
    
  

    
  }
  
  //Checks Extension Support
  void checkInstanceExtensionSupport(const char** glfwExtensionNames, const uint32_t glfwExtensionCount ){
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    
    std::unordered_set<std::string> search_set;
    for (const auto extension: extensions) {
      
      search_set.insert(extension.extensionName);
    }




    for(uint32_t i = 0; i<glfwExtensionCount; i++){
      //FIRSTLY REQUIRED VULKAN EXTENSIONOS LIKE SURFACE...
      if(search_set.find(glfwExtensionNames[i])==search_set.end()){
        throw std::runtime_error{"Required GLFW VULKAN EXTENSIONS Have Not Been Found!"};
      }
    }

    std::cout << ">>>Required Extensions Are Available!\n";

    //Append Optional Extensions Here!
  }

    bool checkVailidationLayerSupoort(){
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    std::cout << "Available Layers: \n";
    for (const auto layer : availableLayers) {
      std::cout << "\t" << layer.layerName << std::endl;
    }

    return false;

    }

  void mainLoop() {
    while(!glfwWindowShouldClose(window)) {
      glfwPollEvents();
    }
  }


  void cleanup() {

    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);

    glfwTerminate();
  }
};

int main() {
  HelloTriangleApplication app;

  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
