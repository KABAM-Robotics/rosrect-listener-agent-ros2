#include <gtest/gtest.h>
#include <fstream>
#include <rosrect-listener-agent/backend_api.h>

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams
using namespace ::pplx;                     // PPLX for tasks
using namespace web::json;                  // JSON features

// Log file settings
std::string package_path = ament_index_cpp::get_package_prefix("rosrect-listener-agent");
int install_pos = package_path.find("install");
std::string log_name = (package_path.replace(install_pos, 7, "src")) + "/test/logs/logData";
std::string log_ext = ".json";
int log_id = 0;

// Utility function to clean up log files
void logCleanup()
{
  bool fileRemoveError = false;

  while (!fileRemoveError)
  {
    // Get filename
    log_id++;
    std::string filename = log_name + std::to_string(log_id) + log_ext;
    // std::cout << "Trying to remove file: " << filename << std::endl;
    // Remove file
    fileRemoveError = remove(filename.c_str());
  }
  log_id = 0;
}

// Create test object
BackendApi api_instance;

// Create sample log  
std::vector<std::vector<std::string>> sample_log;
std::string time_str = "2020-04-14T06:38:06Z";
std::string level = "Error";
std::string cflag = "false";
std::string module = "Null";
std::string source = "bt_navigator";
std::string message = "Navigation failed";
std::string description = "Null";
std::string resolution = "Null"; 
std::string event_id_str = "Sample id";

// Hold the record
std::vector<std::string> event_details;

TEST(BackEndApiTestSuite, pushTest)
{
  // Push to details
  event_details.push_back(time_str);
  event_details.push_back(level);
  event_details.push_back(cflag);
  event_details.push_back(module);
  event_details.push_back(source);
  event_details.push_back(message);
  event_details.push_back(description);
  event_details.push_back(resolution);
  event_details.push_back(event_id_str);
    
  // Push to sample_log
  sample_log.push_back(event_details);

  // Push event
  api_instance.push_event_log(sample_log);

  // Get log file
  int log_id = 1;
  std::string filename = log_name + std::to_string(log_id) + log_ext;

  // Check if file exists
  std::ifstream infile(filename);
  bool fileflag = infile.good();  
  ASSERT_TRUE(fileflag);

}

TEST(BackEndApiTestSuite, jsonTest)
{
  json::value eventJson;
  eventJson = api_instance.create_event_log(sample_log);

  // Check if returned value is an array
  bool arrayFlag = eventJson.is_array();
  ASSERT_TRUE(arrayFlag);

  // Check if it has the right fields according to schema
  bool fieldFlag = true;
  // List all field names
  std::vector<std::string> fieldNames;  
  fieldNames.push_back("Compounding");
  fieldNames.push_back("Date/Time");
  fieldNames.push_back("Description");
  fieldNames.push_back("Level");
  fieldNames.push_back("Message");
  fieldNames.push_back("Module");
  fieldNames.push_back("QID");
  fieldNames.push_back("Resolution");
  fieldNames.push_back("RobotEvent_ID");
  fieldNames.push_back("Source");

  // Loop through field names to see if they exist with the correct data type
  for(int idx=0; idx<fieldNames.size(); idx++){
    fieldFlag = eventJson[0].has_string_field(fieldNames[idx]);
    if(fieldFlag == false){
      break;
    }
  }
  ASSERT_TRUE(fieldFlag);
}

/* Error classification features in development below

TEST(BackEndApiTestSuite, ecsHitTest)
{
  // Sample message
  std::string msgText = "Rotate recovery can't rotate in place because there is a potential collision. Cost: -1.00";
  
  // Check with ecs
  json::value msgInfo;
  msgInfo = api_instance.check_error_classification(msgText);

  // Check if it has the right fields according to schema
  bool fieldFlag = true;
  // List all field names
  std::vector<std::string> fieldNames;  
  fieldNames.push_back("error_code");
  fieldNames.push_back("error_level");
  fieldNames.push_back("compounding_flag");
  fieldNames.push_back("error_module");
  fieldNames.push_back("error_source");
  fieldNames.push_back("error_text");
  fieldNames.push_back("error_description");
  fieldNames.push_back("error_resolution");

  // Loop through field names to see if they exist with the correct data type
  for(int idx=0; idx<fieldNames.size(); idx++){
    if(fieldNames[idx] == "compounding_flag"){
      fieldFlag = msgInfo.has_boolean_field(fieldNames[idx]);
    }
    else{
      fieldFlag = msgInfo.has_string_field(fieldNames[idx]);
    }
    
    if(fieldFlag == false){
      break;
    }
  }
  ASSERT_TRUE(fieldFlag);
}

TEST(BackEndApiTestSuite, ecsMissTest)
{
  // Sample message
  std::string msgText = "Dummy message. Won't be available in ECS.";
  
  // Check with ecs
  json::value msgInfo;
  msgInfo = api_instance.check_error_classification(msgText);

  // Check if null value is returned
  bool nullFlag = msgInfo.is_null();  
  ASSERT_TRUE(nullFlag);
}
*/

int main(int argc, char **argv)
{
  // Cleanup
  logCleanup();

  // Start tests
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}