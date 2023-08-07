#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkGDCMImageIO.h"
#include "itkMetaDataObject.h"

#include <map>

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        std::cout << "Usage " << argv[0] << "filename\n";
        return -1;
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

    using Map = std::map <std::string, std::string>;
    Map tag_valueMap;
    tag_valueMap.insert(std::pair<std::string, std::string>("0008|0080", "Institution"));
    tag_valueMap.insert(std::pair<std::string, std::string>("0008|0081", "Institution Address"));
    tag_valueMap.insert(std::pair<std::string, std::string>("0008|1048", "Dr. John"));
    tag_valueMap.insert(std::pair<std::string, std::string>("0008|1070", "Operator"));
    tag_valueMap.insert(std::pair<std::string, std::string>("0010|0010", "John Doe"));
    tag_valueMap.insert(std::pair<std::string, std::string>("0010|0020", "000001"));
    
    Map::iterator itr = tag_valueMap.begin();
    Map::iterator end = tag_valueMap.end();

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
