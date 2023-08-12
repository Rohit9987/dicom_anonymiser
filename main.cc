#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkGDCMImageIO.h"
#include "itkMetaDataObject.h"

#include "itkImageSeriesReader.h"
#include "itkImageSeriesWriter.h"
#include "itkGDCMSeriesFileNames.h"

#include <json/json.h>
#include <json/value.h>

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <filesystem>       // c++17
namespace fs = std::filesystem;


using Map = std::map <std::string, std::string>;

std::map<std::string, std::string> jsonToMap(std::string filename)
{
    Json::Value root;
    Json::Reader jsonReader;

    std::ifstream in(filename);
    if(!in)
    {
        std::string errReason("Cannot open the file ");
        errReason += filename;
        throw std::runtime_error(errReason);
    }

    bool parsingSuccessful = jsonReader.parse(in, root);
    if(!parsingSuccessful)
    {
        std::string errReason("Error parsing the string\n");
        throw std::runtime_error(errReason);
    }

    Map map_;

    Json::Value::Members names = root.getMemberNames();
    for(size_t index = 0; index < names.size(); ++index)
    {
        std::string key = names[index];
        std::string value = root[key].asString();
        map_.insert(make_pair(key, value));
    }

    return map_;
}

int seriesAnonymiser(const char* inputDirectory)
{
    using PixelType = signed short;
    constexpr unsigned int Dimension = 3;
    using ImageType = itk::Image<PixelType, Dimension>;
    using ReaderType = itk::ImageSeriesReader<ImageType>;

    using ImageIOType = itk::GDCMImageIO;
    ImageIOType::Pointer gdcmIO = ImageIOType::New();

    using NamesGeneratorType = itk::GDCMSeriesFileNames;
    auto namesGenerator = NamesGeneratorType::New();
    namesGenerator->SetInputDirectory(inputDirectory);
    const ReaderType::FileNamesContainer& filenames = namesGenerator->GetInputFileNames();
    
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetImageIO(gdcmIO);
    reader->SetFileNames(filenames);

    try{ reader->Update();}
    catch(itk::ExceptionObject& ex) { std::cerr << ex << '\n'; return -1; }

    std::string datetime = itksys::SystemTools::GetCurrentDateTime("%d_%b_%Y_%H_%M_%S"); 
    std::string outputDirectory = "./output_" + datetime;
    itksys::SystemTools::MakeDirectory(outputDirectory);
    
    constexpr unsigned int OutputDimension = 2;
    using Image2DType = itk::Image<PixelType, OutputDimension>;
    using SeriesWriterType = itk::ImageSeriesWriter<ImageType, Image2DType>;

    SeriesWriterType::Pointer seriesWriter = SeriesWriterType::New();
    seriesWriter->SetInput(reader->GetOutput());
    seriesWriter->SetImageIO(gdcmIO);
    
    namesGenerator->SetOutputDirectory(outputDirectory);
    seriesWriter->SetFileNames(namesGenerator->GetOutputFileNames());
    seriesWriter->SetMetaDataDictionaryArray(reader->GetMetaDataDictionaryArray());
    
    try { seriesWriter->Update();}
    catch(itk::ExceptionObject& ex) { std::cerr << ex << '\n'; return -1; }

    return 0;
}

int main(int argc, char **argv)
{
    if(argc < 3)
    {
        std::cout << "Usage: " << argv[0] << " dicomfilename tagkeyfile\n";
        return -1;
    }


    if(itksys::SystemTools::FileIsDirectory(argv[1]))
    {
        std::cout << "Processing dicom series\n";
        seriesAnonymiser(argv[1]);
        return 0;
    }
    else 
    {
        std::cout << "Processing dicom\n";
    }

    Map map_; 
    try
    {
        map_ = jsonToMap(argv[2]);
    }
    catch(const std::exception& ex)
    {
        std::cerr << ex.what() << "\n";
        return -2;
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
