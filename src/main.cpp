#include <string>
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

VkResult CreateDebugUtilMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT*  pCreateInfo, const  VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger){
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  if(func != nullptr){
    return func(instance,pCreateInfo,pAllocator,pDebugMessenger);
  }else{
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

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
  VkDebugUtilsMessengerEXT debugMessenger;

  void initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
  }
  void initVulkan() {
    createInstance();
    setupDebugMessenger();
  }

  void createInstance(){

    if(enableValidationLayers && !checkVailidationLayerSupoort()) {
      throw std::runtime_error{"Validation Layers Requested, but are not available!"};
    }

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
    
    
    auto extensions = getRequiredExtensions();

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if(enableValidationLayers){
      createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
      createInfo.ppEnabledLayerNames = validationLayers.data();

      populateDebugMessengerCreateInfo(debugCreateInfo);
      createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    }else{
      createInfo.enabledLayerCount = 0;
      createInfo.pNext = nullptr;
    }

    
   
    if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS){
      throw std::runtime_error{"Failed to Create Vulkan Instance!"};
    }
    
  

    
  }

  void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo){
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional
    //

  }

  void setupDebugMessenger(){
    if(!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);

    if(CreateDebugUtilMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
      throw std::runtime_error{"Failed to set up debug Messenger!"};
    }

  }

  std::vector<const char*> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
  }
  
  //Checks Extension Support
  bool checkInstanceExtensionSupport(const char** glfwExtensionNames, const uint32_t glfwExtensionCount ){
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
        return false;
      }
    }

    return true;
    //Append Optional Extensions Here!
  }

    bool checkVailidationLayerSupoort(){
      uint32_t layerCount = 0;
      vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

      std::vector<VkLayerProperties> availableLayers(layerCount);
      vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
      
      std::unordered_set<std::string> search_set{layerCount};
      for (const auto layer : availableLayers) {
        search_set.insert(layer.layerName);
      }

      for(const char* layerName: validationLayers){
        if(search_set.find(layerName) == search_set.end()){
          return false;
        }
      }
      std::cout << ">>>All Layers Available\n";
      return true;


      

    }

  void mainLoop() {
    while(!glfwWindowShouldClose(window)) {
      glfwPollEvents();
    }
  }

  static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
  }



  void cleanup() {
    if(enableValidationLayers){
      DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }
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
