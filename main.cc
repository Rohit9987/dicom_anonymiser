#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkGDCMImageIO.h"
#include "itkMetaDataObject.h"

#include <json/json.h>
#include <json/value.h>

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <filesystem>       // c++17
namespace fs = std::filesystem;



int main(int argc, char **argv)
{
    if(argc < 3)
    {
        std::cout << "Usage " << argv[0] << "filename tagkeyfile\n";
        return -1;
    }

    const std::string pathString = argv[1];
    const fs::path path(pathString);

    std::error_code ec;
    if(fs::is_directory(path, ec))
    {
        std::cout << "Processing dicom series\n";
    }
    if(fs::is_regular_file(path, ec))
    {
        std::cout << "Processing dicom\n";
    }
    if(ec)
    {
        std::cerr << "Error in " << argv[1] << "\n" << ec.message();
    }



    Json::Value root;
    Json::Reader jsonReader;

    std::ifstream in(argv[2]);

    if(!in)
    {
        std::string errReason("Cannot open the file");
        errReason += argv[2];
        
        throw std::domain_error(errReason);
    }

    bool parsingSuccessful = jsonReader.parse(in, root);

    if(!parsingSuccessful)
    {
        std::cout << "Error parsing the string\n";
        return -1;
    }


    using Map = std::map <std::string, std::string>;
    Map map_;

    Json::Value::Members names = root.getMemberNames();
    for(size_t index = 0; index < names.size(); ++index)
    {
        std::string key = names[index];
        std::string value = root[key].asString();
        map_.insert(make_pair(key, value));
    }
     


    using PixelType = signed short;
    constexpr unsigned int Dimension = 2;

    using ImageType = itk::Image<PixelType, Dimension>;
    
    using ReaderType = itk::ImageFileReader<ImageType>;
    ReaderType::Pointer reader =  ReaderType::New();

    using ImageIOType = itk::GDCMImageIO;
    ImageIOType::Pointer dicomIO = ImageIOType::New();
    reader->SetFileName(argv[1]);
    reader->SetImageIO(dicomIO);

    try
    {
        reader->Update();
    }
    catch(itk::ExceptionObject& ex)
    {
        std::cerr << ex <<std::endl;
        return -2;
    }


    ImageType::Pointer inputImage = reader->GetOutput();
    using MetaDataStringType = itk::MetaDataObject<std::string>;

    using DictionaryType = itk::MetaDataDictionary;
    //DictionaryType& dictionary = dicomIO->GetMetaDataDictionary();
    DictionaryType& dictionary = inputImage->GetMetaDataDictionary();


    /*
        tagkey = 0010|0020
        labelId = Patient's Name
        tagvalue = actual name

        0008|0080 = Institution Name        - leave blank
        0008|0081 = Institution Address     - leave blank

        0008|1048 = Physician(s) of Record  - leave blank
        0008|1070 = Operator's Name         - leave blank
        
        0010|0010 = Patient's Name          - John Doe
        0010|0020 = Patient's Id            - Ask from user (001234)

    */

    /*
    using Map = std::map <std::string, std::string>;
    Map tag_valueMap;
    tag_valueMap.insert(std::pair<std::string, std::string>("0008|0080", "Institution"));
    tag_valueMap.insert(std::pair<std::string, std::string>("0008|0081", "Institution Address"));
    tag_valueMap.insert(std::pair<std::string, std::string>("0008|1048", "Dr. John"));
    tag_valueMap.insert(std::pair<std::string, std::string>("0008|1070", "Operator"));
    tag_valueMap.insert(std::pair<std::string, std::string>("0010|0010", "John Doe"));
    tag_valueMap.insert(std::pair<std::string, std::string>("0010|0020", "000001"));
    */
    
    Map::iterator itr = map_.begin();
    Map::iterator end = map_.end();

    while(itr != end)
    {
        itk::EncapsulateMetaData<std::string>(dictionary, itr->first, itr->second);
        itr++;
    }



    using WriterType = itk::ImageFileWriter<ImageType>;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName("output.dcm");
    writer->SetInput(reader->GetOutput());
    writer->SetImageIO(dicomIO);
    writer->SetMetaDataDictionary(dictionary);

    try
    {
        writer->Update();
    }
    catch(itk::ExceptionObject& ex)
    {
        std::cerr << ex <<std::endl;
        return -3;
    }

    return 0;
}
