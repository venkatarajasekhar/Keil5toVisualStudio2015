#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <Windows.h>
#include <direct.h>
#include "tinyxml2.h"
#include "Groups.h"

using namespace std;
using namespace std::tr2::sys;
using namespace tinyxml2;

string keilProjPath;				// Keil 项目工程路径
string vsSlnPath;					// VisualStudio 解决方案路径
string vsProjPath;					// VisualStudio 项目工程路径
string projName;					// 项目工程名
string keilInstaPath;				// Keil 安装路径
string deviceName;					// 芯片设备名
string incPath;				// Keil 项目工程组

void createSln(string destinPath, string projectName);
void createKeilProjectProps(string destinPath, string include, string define);
void createVcxproj(string destinPath, string projectName, vector<Groups> &itemGroup);
void createFilters(string destinPath, vector<Groups> &itemGroup);
void createUser(string destinPath);
string GBKToUTF8(const std::string& strGBK);

void schemaVersion1_1(void);
void schemaVersion2_1(void);

int main(int argc, char *argv[])
{
	cout << "################################KeiltoVisualStudio2015################################" << endl;

	/* 解析Keil传递参数 */
	for (int i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "-p"))
		{
			keilProjPath = argv[++i];
			keilProjPath = keilProjPath.substr(0, keilProjPath.size());
			cout << "Keil5项目路径：" << keilProjPath << endl;
		}
		if (!strcmp(argv[i], "-k"))
		{
			keilInstaPath = argv[++i];
			keilInstaPath = keilInstaPath.substr(0, keilInstaPath.size() - 1);
			cout << "Keil5安装路径：" << keilInstaPath << endl;
		}
		if (!strcmp(argv[i], "-d"))
		{
			deviceName = argv[++i];
			deviceName = deviceName.substr(0, deviceName.size());
			cout << "芯片型号：" << deviceName << endl;
		}
		if (!strcmp(argv[i], "-j"))
		{
			incPath = argv[++i];
			incPath = incPath.substr(0, incPath.size());
			cout << "Keil库路径：" << incPath << endl;
		}
	}
	/* 创建VisualStudio工程存放文件夹 */
	int error;

	projName = path(keilProjPath).stem().string();
	vsSlnPath = path(keilProjPath).parent_path().string() + "\\VisualStudio";
	vsProjPath = vsSlnPath + "\\" + projName;

	error = _mkdir(vsSlnPath.c_str());
	switch (error)
	{
	case 0:
		cout << "创建VisualStudio文件夹" << endl;
		break;
	case -1:
		cout << "存在VisualStudio文件夹" << endl;
		break;
	default:
		cout << "创建VisualStudio发生错误" << error << endl;
		return 1;
		break;
	}
	error = _mkdir(vsProjPath.c_str());
	switch (error)
	{
	case 0:
		cout << "创建" << projName << "文件夹" << endl;
		break;
	case -1:
		cout << "存在" << projName << "文件夹" << endl;
		break;
	default:
		cout << "创建" << projName << "发生错误：" << error << endl;
		return 2;
		break;
	}
	/* 解析Keil配置版本 */
	tinyxml2::XMLDocument projDoc;

	if (projDoc.LoadFile(keilProjPath.c_str()))
	{
		cout << "无法加载Keil工程配置" << endl;
		return 3;
	}
	else
	{
		cout << "成功加载Keil工程配置" << endl;
	}

	string schemaVersion;
	schemaVersion = projDoc.FirstChildElement("Project")->FirstChildElement("SchemaVersion")->GetText();
	if (schemaVersion == "1.1")
	{
		schemaVersion1_1();
	}
	if (schemaVersion == "2.1")
	{
		schemaVersion2_1();
	}

	cout << "解决方案路径：" << vsSlnPath << endl;
	cout << "######################################################################################" << endl;
	
	system("pause");
	return 0;
}

void createSln(string destinPath, string projectName)
{
	/* 创建 .sln 文件*/
	fstream sln(destinPath.c_str(), ios::in | ios::out | ios::trunc);

	if (!sln.bad())
	{
		sln << endl;
		sln << "Microsoft Visual Studio Solution File, Format Version 12.00" << endl;
		sln << "# Visual Studio 14" << endl;
		sln << "VisualStudioVersion = 14.0.25123.0" << endl;
		sln << "MinimumVisualStudioVersion = 10.0.40219.1" << endl;
		sln << "Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \""
			<< GBKToUTF8(projectName) << "\", \"" << GBKToUTF8(projectName) << "\\" << GBKToUTF8(projectName)
			<< ".vcxproj\", \"{2B12F5AC-5B69-4B98-9500-48ABA0FF8E8A}" << endl;
		sln << "EndProject" << endl;
		sln << "Global" << endl;
		sln << "\tGlobalSection(SolutionConfigurationPlatforms) = preSolution" << endl;
		sln << "\t\tDebug|x64 = Debug|x64" << endl;
		sln << "\t\tDebug|x86 = Debug|x86" << endl;
		sln << "\t\tRelease|x64 = Release|x64" << endl;
		sln << "\t\tRelease|x86 = Release|x86" << endl;
		sln << "\tEndGlobalSection" << endl;
		sln << "\tGlobalSection(ProjectConfigurationPlatforms) = postSolution" << endl;
		sln << "\t\t{2B12F5AC-5B69-4B98-9500-48ABA0FF8E8A}.Debug|x64.ActiveCfg = Debug|x64" << endl;
		sln << "\t\t{2B12F5AC-5B69-4B98-9500-48ABA0FF8E8A}.Debug|x64.Build.0 = Debug|x64" << endl;
		sln << "\t\t{2B12F5AC-5B69-4B98-9500-48ABA0FF8E8A}.Debug|x86.ActiveCfg = Debug|Win32" << endl;
		sln << "\t\t{2B12F5AC-5B69-4B98-9500-48ABA0FF8E8A}.Debug|x86.Build.0 = Debug|Win32" << endl;
		sln << "\t\t{2B12F5AC-5B69-4B98-9500-48ABA0FF8E8A}.Release|x64.ActiveCfg = Release|x64" << endl;
		sln << "\t\t{2B12F5AC-5B69-4B98-9500-48ABA0FF8E8A}.Release|x64.Build.0 = Release|x64" << endl;
		sln << "\t\t{2B12F5AC-5B69-4B98-9500-48ABA0FF8E8A}.Release|x86.ActiveCfg = Release|Win32" << endl;
		sln << "\t\t{2B12F5AC-5B69-4B98-9500-48ABA0FF8E8A}.Release|x86.Build.0 = Release|Win32" << endl;
		sln << "\tEndGlobalSection" << endl;
		sln << "\tGlobalSection(SolutionProperties) = preSolution" << endl;
		sln << "\t\tHideSolutionNode = FALSE" << endl;
		sln << "\tEndGlobalSection" << endl;
		sln << "EndGlobal" << endl;
	}

	sln.close();
}

void createKeilProjectProps(string destinPath, string include, string define)
{
	char* pBuf = new char[include.size()];
	include.copy(pBuf, include.size());

	/* 创建 Keil5Project.props 文件 */
	fstream props(destinPath, ios::in | ios::out | ios::trunc);

	if (!props.bad())
	{
		props << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << endl;
		props << "<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">" << endl;
		props << "  <ImportGroup Label=\"PropertySheets\" />" << endl;
		props << "  <PropertyGroup Label=\"UserMacros\" />" << endl;
		props << "  <PropertyGroup>" << endl;
		props << "  <IncludePath>" << GBKToUTF8(include) << "</IncludePath>" << endl;
		props << "  </PropertyGroup>" << endl;
		props << "  <ItemDefinitionGroup />" << endl;
		props << "  <ItemGroup />" << endl;
		props << "  <ItemDefinitionGroup>" << endl;
		props << "    <ClCompile>" << endl;
		props << "      <PreprocessorDefinitions>_MBCS;%(PreprocessorDefinitions);" << define << "</PreprocessorDefinitions>" << endl;
		props << "    </ClCompile>" << endl;
		props << "  </ItemDefinitionGroup>" << endl;
		props << "</Project>" << endl;
	}

	props.close();
}

void createVcxproj(string destinPath, string projectName, vector<Groups> &itemGroup)
{
	/* 创建 .vcxproj 文件 */
	fstream vcxproj(destinPath, ios::in | ios::out | ios::trunc);

	if (!vcxproj.bad())
	{
		vcxproj << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << endl;
		vcxproj << "<Project DefaultTargets=\"Build\" ToolsVersion=\"14.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">" << endl;
		vcxproj << "  <ItemGroup Label=\"ProjectConfigurations\">" << endl;
		vcxproj << "    <ProjectConfiguration Include=\"Debug|Win32\">" << endl;
		vcxproj << "      <Configuration>Debug</Configuration>" << endl;
		vcxproj << "      <Platform>Win32</Platform>" << endl;
		vcxproj << "    </ProjectConfiguration>" << endl;
		vcxproj << "    <ProjectConfiguration Include=\"Release|Win32\">" << endl;
		vcxproj << "      <Configuration>Release</Configuration>" << endl;
		vcxproj << "      <Platform>Win32</Platform>" << endl;
		vcxproj << "    </ProjectConfiguration>" << endl;
		vcxproj << "    <ProjectConfiguration Include=\"Debug|x64\">" << endl;
		vcxproj << "      <Configuration>Debug</Configuration>" << endl;
		vcxproj << "      <Platform>x64</Platform>" << endl;
		vcxproj << "    </ProjectConfiguration>" << endl;
		vcxproj << "    <ProjectConfiguration Include=\"Release|x64\">" << endl;
		vcxproj << "      <Configuration>Release</Configuration>" << endl;
		vcxproj << "      <Platform>x64</Platform>" << endl;
		vcxproj << "    </ProjectConfiguration>" << endl;
		vcxproj << "  </ItemGroup>" << endl;
		vcxproj << "  <PropertyGroup Label=\"Globals\">" << endl;
		vcxproj << "    <ProjectGuid>{2B12F5AC-5B69-4B98-9500-48ABA0FF8E8A}</ProjectGuid>" << endl;
		vcxproj << "    <RootNamespace>" << GBKToUTF8(projectName) << "</RootNamespace>" << endl;
		vcxproj << "    <WindowsTargetPlatformVersion>8.1</WindowsTargetPlatformVersion>" << endl;
		vcxproj << "  </PropertyGroup>" << endl;
		vcxproj << "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\" />" << endl;
		vcxproj << "  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\" Label=\"Configuration\">" << endl;
		vcxproj << "    <ConfigurationType>Application</ConfigurationType>" << endl;
		vcxproj << "    <UseDebugLibraries>true</UseDebugLibraries>" << endl;
		vcxproj << "    <PlatformToolset>v140</PlatformToolset>" << endl;
		vcxproj << "    <CharacterSet>MultiByte</CharacterSet>" << endl;
		vcxproj << "  </PropertyGroup>" << endl;
		vcxproj << "  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\" Label=\"Configuration\">" << endl;
		vcxproj << "    <ConfigurationType>Application</ConfigurationType>" << endl;
		vcxproj << "    <UseDebugLibraries>false</UseDebugLibraries>" << endl;
		vcxproj << "    <PlatformToolset>v140</PlatformToolset>" << endl;
		vcxproj << "    <WholeProgramOptimization>true</WholeProgramOptimization>" << endl;
		vcxproj << "    <CharacterSet>MultiByte</CharacterSet>" << endl;
		vcxproj << "  </PropertyGroup>" << endl;
		vcxproj << "  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\" Label=\"Configuration\">" << endl;
		vcxproj << "    <ConfigurationType>Application</ConfigurationType>" << endl;
		vcxproj << "    <UseDebugLibraries>true</UseDebugLibraries>" << endl;
		vcxproj << "    <PlatformToolset>v140</PlatformToolset>" << endl;
		vcxproj << "    <CharacterSet>MultiByte</CharacterSet>" << endl;
		vcxproj << "  </PropertyGroup>" << endl;
		vcxproj << "  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\" Label=\"Configuration\">" << endl;
		vcxproj << "    <ConfigurationType>Application</ConfigurationType>" << endl;
		vcxproj << "    <UseDebugLibraries>false</UseDebugLibraries>" << endl;
		vcxproj << "    <PlatformToolset>v140</PlatformToolset>" << endl;
		vcxproj << "    <WholeProgramOptimization>true</WholeProgramOptimization>" << endl;
		vcxproj << "    <CharacterSet>MultiByte</CharacterSet>" << endl;
		vcxproj << "  </PropertyGroup>" << endl;
		vcxproj << "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\" />" << endl;
		vcxproj << "  <ImportGroup Label=\"ExtensionSettings\">" << endl;
		vcxproj << "  </ImportGroup>" << endl;
		vcxproj << "  <ImportGroup Label=\"Shared\">" << endl;
		vcxproj << "  </ImportGroup>" << endl;
		vcxproj << "  <ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">" << endl;
		vcxproj << "    <Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />" << endl;
		vcxproj << "    <Import Project=\"Keil5Project.props\" />" << endl;
		vcxproj << "  </ImportGroup>" << endl;
		vcxproj << "  <ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">" << endl;
		vcxproj << "    <Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />" << endl;
		vcxproj << "    <Import Project=\"Keil5Project.props\" />" << endl;
		vcxproj << "  </ImportGroup>" << endl;
		vcxproj << "  <ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">" << endl;
		vcxproj << "    <Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />" << endl;
		vcxproj << "    <Import Project=\"Keil5Project.props\" />" << endl;
		vcxproj << "  </ImportGroup>" << endl;
		vcxproj << "  <ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">" << endl;
		vcxproj << "    <Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />" << endl;
		vcxproj << "    <Import Project=\"Keil5Project.props\" />" << endl;
		vcxproj << "  </ImportGroup>" << endl;
		vcxproj << "  <PropertyGroup Label=\"UserMacros\" />" << endl;
		vcxproj << "  <PropertyGroup />" << endl;
		vcxproj << "  <ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">" << endl;
		vcxproj << "    <ClCompile>" << endl;
		vcxproj << "      <WarningLevel>Level3</WarningLevel>" << endl;
		vcxproj << "      <Optimization>Disabled</Optimization>" << endl;
		vcxproj << "      <SDLCheck>false</SDLCheck>" << endl;
		vcxproj << "    </ClCompile>" << endl;
		vcxproj << "  </ItemDefinitionGroup>" << endl;
		vcxproj << "  <ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">" << endl;
		vcxproj << "    <ClCompile>" << endl;
		vcxproj << "      <WarningLevel>Level3</WarningLevel>" << endl;
		vcxproj << "      <Optimization>Disabled</Optimization>" << endl;
		vcxproj << "      <SDLCheck>false</SDLCheck>" << endl;
		vcxproj << "    </ClCompile>" << endl;
		vcxproj << "  </ItemDefinitionGroup>" << endl;
		vcxproj << "  <ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">" << endl;
		vcxproj << "    <ClCompile>" << endl;
		vcxproj << "      <WarningLevel>Level3</WarningLevel>" << endl;
		vcxproj << "      <Optimization>MaxSpeed</Optimization>" << endl;
		vcxproj << "      <FunctionLevelLinking>true</FunctionLevelLinking>" << endl;
		vcxproj << "      <IntrinsicFunctions>true</IntrinsicFunctions>" << endl;
		vcxproj << "      <SDLCheck>true</SDLCheck>" << endl;
		vcxproj << "    </ClCompile>" << endl;
		vcxproj << "    <Link>" << endl;
		vcxproj << "      <EnableCOMDATFolding>true</EnableCOMDATFolding>" << endl;
		vcxproj << "      <OptimizeReferences>true</OptimizeReferences>" << endl;
		vcxproj << "    </Link>" << endl;
		vcxproj << "  </ItemDefinitionGroup>" << endl;
		vcxproj << "  <ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">" << endl;
		vcxproj << "    <ClCompile>" << endl;
		vcxproj << "      <WarningLevel>Level3</WarningLevel>" << endl;
		vcxproj << "      <Optimization>MaxSpeed</Optimization>" << endl;
		vcxproj << "      <FunctionLevelLinking>true</FunctionLevelLinking>" << endl;
		vcxproj << "      <IntrinsicFunctions>true</IntrinsicFunctions>" << endl;
		vcxproj << "      <SDLCheck>true</SDLCheck>" << endl;
		vcxproj << "    </ClCompile>" << endl;
		vcxproj << "    <Link>" << endl;
		vcxproj << "      <EnableCOMDATFolding>true</EnableCOMDATFolding>" << endl;
		vcxproj << "      <OptimizeReferences>true</OptimizeReferences>" << endl;
		vcxproj << "    </Link>" << endl;
		vcxproj << "  </ItemDefinitionGroup>" << endl;
		vcxproj << "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\" />" << endl;
		vcxproj << "  <ImportGroup Label=\"ExtensionTargets\">" << endl;
		vcxproj << "  </ImportGroup>" << endl;
		vcxproj << "</Project>" << endl;
	}

	vcxproj.close();

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement *xmlItemGroup;
	tinyxml2::XMLElement *xmlClInclude;
	tinyxml2::XMLElement *xmlClCompile;

	doc.LoadFile(destinPath.c_str());

	xmlItemGroup = doc.NewElement("ItemGroup");
	doc.FirstChildElement("Project")->InsertEndChild(xmlItemGroup);
	for (int i = 0; i < itemGroup.size(); i++)
	{
		for (int j = 0; j < itemGroup[i].filePath.size(); j++)
		{
			if (path(itemGroup[i].filePath[j]).extension().string() == ".h" |
				path(itemGroup[i].filePath[j]).extension().string() == ".hpp")
			{
				xmlClInclude = doc.NewElement("ClInclude");
				xmlClInclude->SetAttribute("Include", GBKToUTF8(itemGroup[i].filePath[j]).c_str());
				xmlItemGroup->InsertEndChild(xmlClInclude);
			}
		}
	}

	xmlItemGroup = doc.NewElement("ItemGroup");
	doc.FirstChildElement("Project")->InsertEndChild(xmlItemGroup);
	for (int i = 0; i < itemGroup.size(); i++)
	{
		for (int j = 0; j < itemGroup[i].filePath.size(); j++)
		{
			if (path(itemGroup[i].filePath[j]).extension().string() == ".c" |
				path(itemGroup[i].filePath[j]).extension().string() == ".cpp" |
				path(itemGroup[i].filePath[j]).extension().string() == ".s")
			{
				xmlClCompile = doc.NewElement("ClCompile");
				xmlClCompile->SetAttribute("Include", GBKToUTF8(itemGroup[i].filePath[j]).c_str());
				xmlItemGroup->InsertEndChild(xmlClCompile);
			}
		}
	}

	doc.SaveFile(destinPath.c_str(), false);
}

void createFilters(string destinPath, vector<Groups> &itemGroup)
{
	/* 创建 .filters 文件 */
	fstream filters(destinPath, ios::in | ios::out | ios::trunc);

	if (!filters.bad())
	{
		filters << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << endl;
		filters << "<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">" << endl;
		filters << "</Project>" << endl;
	}

	filters.close();

	/* .filters */
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement *xmlItemGroup;
	tinyxml2::XMLElement *xmlFilter;
	tinyxml2::XMLElement *xmlClInclude;
	tinyxml2::XMLElement *xmlClCompile;

	doc.LoadFile(destinPath.c_str());

	if (itemGroup.size() != 0)
	{
		xmlItemGroup = doc.NewElement("ItemGroup");
		doc.FirstChildElement("Project")->InsertEndChild(xmlItemGroup);
		for (int i = 0; i < itemGroup.size(); i++)
		{
			xmlFilter = doc.NewElement("Filter");
			xmlFilter->SetAttribute("Include", GBKToUTF8(itemGroup[i].groupName).c_str());
			xmlItemGroup->InsertEndChild(xmlFilter);
		}

		xmlItemGroup = doc.NewElement("ItemGroup");
		doc.FirstChildElement("Project")->InsertEndChild(xmlItemGroup);
		for (int i = 0; i < itemGroup.size(); i++)
		{
			for (int j = 0; j < itemGroup[i].filePath.size(); j++)
			{
				if (path(itemGroup[i].filePath[j]).extension().string() == ".h" |
					path(itemGroup[i].filePath[j]).extension().string() == ".hpp")
				{
					xmlClInclude = doc.NewElement("ClInclude");
					xmlClInclude->SetAttribute("Include", GBKToUTF8(itemGroup[i].filePath[j]).c_str());
					xmlItemGroup->InsertEndChild(xmlClInclude);
					xmlFilter = doc.NewElement("Filter");
					xmlFilter->SetText(GBKToUTF8(itemGroup[i].groupName).c_str());
					xmlClInclude->InsertEndChild(xmlFilter);
				}
			}
		}

		xmlItemGroup = doc.NewElement("ItemGroup");
		doc.FirstChildElement("Project")->InsertEndChild(xmlItemGroup);
		for (int i = 0; i < itemGroup.size(); i++)
		{
			for (int j = 0; j < itemGroup[i].filePath.size(); j++)
			{
				if (path(itemGroup[i].filePath[j]).extension().string() == ".c" |
					path(itemGroup[i].filePath[j]).extension().string() == ".cpp" |
					path(itemGroup[i].filePath[j]).extension().string() == ".s")
				{
					xmlClCompile = doc.NewElement("ClCompile");
					xmlClCompile->SetAttribute("Include", GBKToUTF8(itemGroup[i].filePath[j]).c_str());
					xmlItemGroup->InsertEndChild(xmlClCompile);
					xmlFilter = doc.NewElement("Filter");
					xmlFilter->SetText(GBKToUTF8(itemGroup[i].groupName).c_str());
					xmlClCompile->InsertEndChild(xmlFilter);
				}
			}
		}
	}

	doc.SaveFile(destinPath.c_str(), false);
}

void createUser(string destinPath)
{
	/*创建 .user 文件 */
	fstream user(destinPath, ios::in | ios::out | ios::trunc);

	if (!user.bad())
	{
		user << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << endl;
		user << "<Project ToolsVersion=\"14.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">" << endl;
		user << "  <PropertyGroup />" << endl;
		user << "</Project>" << endl;
	}

	user.close();
}

string GBKToUTF8(const std::string& strGBK)
{
	string strUTF8 = "";
	wchar_t *str1;
	int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);
	str1 = new wchar_t[n];
	MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, str1, n);
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
	char * str2 = new char[n];
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
	strUTF8 = str2;
	delete[]str1;
	str1 = NULL;
	delete[]str2;
	str2 = NULL;
	return strUTF8;
}

void schemaVersion1_1(void)
{
	tinyxml2::XMLDocument projDoc;
	projDoc.LoadFile(keilProjPath.c_str());

	vector<Groups> itemGroup;
	tinyxml2::XMLElement *xmlProject = projDoc.FirstChildElement("Project");
	tinyxml2::XMLElement *xmlTaggets = xmlProject->FirstChildElement("Targets");
	tinyxml2::XMLElement *xmlTarget = xmlTaggets->FirstChildElement("Target");
	tinyxml2::XMLElement *xmlGroups = xmlTarget->FirstChildElement("Groups");
	tinyxml2::XMLElement *xmlGroupTemp = xmlGroups->FirstChildElement("Group");
	tinyxml2::XMLElement *xmlFileTemp;

	Groups groupsTemp;
	for (int i = 0; xmlGroupTemp != NULL; i++)
	{
		groupsTemp.groupName = xmlGroupTemp->FirstChildElement("GroupName")->GetText();
		xmlFileTemp = xmlGroupTemp->FirstChildElement("Files");
		if (xmlFileTemp)
		{
			xmlFileTemp = xmlFileTemp->FirstChildElement("File");
		}

		for (int j = 0; xmlFileTemp != NULL; j++)
		{
			groupsTemp.filePath.insert(groupsTemp.filePath.begin() + j, string("..\\.") + xmlFileTemp->FirstChildElement("FilePath")->GetText());
			xmlFileTemp = xmlFileTemp->NextSiblingElement();

		}
		itemGroup.insert(itemGroup.begin() + i, groupsTemp);
		xmlGroupTemp = xmlGroupTemp->NextSiblingElement();
		groupsTemp.filePath.clear();
	}

	string include;
	string define;
	string vendor;
	
	vendor = projDoc.FirstChildElement("Project")->FirstChildElement("Targets")->FirstChildElement("Target")
		->FirstChildElement("TargetOption")->FirstChildElement("TargetCommonOption")->FirstChildElement("Vendor")
		->GetText();

	include += incPath + ";";
	include += incPath + "\\" + vendor + ";";

	tinyxml2::XMLElement *xmlTemp;
	xmlTemp = projDoc.FirstChildElement("Project")->FirstChildElement("Targets")->FirstChildElement("Target")
		->FirstChildElement("TargetOption")->FirstChildElement("Target51")->FirstChildElement("C51")
		->FirstChildElement("VariousControls")->FirstChildElement("Define");
	
	if (xmlTemp->GetText() != NULL)
		define = xmlTemp->GetText();
	
	for (int i = 0; i < define.size(); i++)
	{
		if (define[i] == ',')
			define[i] = ';';
	}
	define = "interrupt;sbit;sfr";

	cout << "生成VisualStudio2015解决方案" << endl;
	createSln(vsSlnPath + "\\" + projName + ".sln", projName);
	createKeilProjectProps(vsProjPath + "\\Keil5Project.props", include, define);
	createVcxproj(vsProjPath + "\\" + projName + ".vcxproj", projName, itemGroup);
	createFilters(vsProjPath + "\\" + projName + ".vcxproj.filters", itemGroup);
	createUser(vsProjPath + "\\" + projName + ".vcxproj.user");
}

void schemaVersion2_1(void)
{
	tinyxml2::XMLDocument projDoc;
	projDoc.LoadFile(keilProjPath.c_str());

	vector<Groups> itemGroup;
	tinyxml2::XMLElement *xmlProject = projDoc.FirstChildElement("Project");
	tinyxml2::XMLElement *xmlTaggets = xmlProject->FirstChildElement("Targets");
	tinyxml2::XMLElement *xmlTarget = xmlTaggets->FirstChildElement("Target");
	tinyxml2::XMLElement *xmlGroups = xmlTarget->FirstChildElement("Groups");
	tinyxml2::XMLElement *xmlGroupTemp = xmlGroups->FirstChildElement("Group");
	tinyxml2::XMLElement *xmlFileTemp;

	Groups groupsTemp;
	for (int i = 0; xmlGroupTemp != NULL; i++)
	{
		groupsTemp.groupName = xmlGroupTemp->FirstChildElement("GroupName")->GetText();
		xmlFileTemp = xmlGroupTemp->FirstChildElement("Files");
		if (xmlFileTemp)
		{
			xmlFileTemp = xmlFileTemp->FirstChildElement("File");
		}

		for (int j = 0; xmlFileTemp != NULL; j++)
		{
			groupsTemp.filePath.insert(groupsTemp.filePath.begin() + j, string("..\\.") + xmlFileTemp->FirstChildElement("FilePath")->GetText());
			xmlFileTemp = xmlFileTemp->NextSiblingElement();

		}
		itemGroup.insert(itemGroup.begin() + i, groupsTemp);
		xmlGroupTemp = xmlGroupTemp->NextSiblingElement();
		groupsTemp.filePath.clear();
	}

	string include;
	string cmsisVer;
	string devPackName;
	string devPackVer;
	string define;

	cmsisVer = projDoc.FirstChildElement("Project")->FirstChildElement("RTE")->FirstChildElement("components")
		->FirstChildElement("component")->FirstChildElement("package")->Attribute("version");
	devPackName = projDoc.FirstChildElement("Project")->FirstChildElement("RTE")->FirstChildElement("files")
		->FirstChildElement("file")->FirstChildElement("package")->Attribute("name");
	devPackVer = projDoc.FirstChildElement("Project")->FirstChildElement("RTE")->FirstChildElement("files")
		->FirstChildElement("file")->FirstChildElement("package")->Attribute("version");

	include += path(keilProjPath).parent_path().string() + "\\RTE;";
	include += path(keilProjPath).parent_path().string() + "\\RTE\\Device\\" + deviceName + ";";
	include += incPath + ";";
	include += keilInstaPath + "\\ARM\\PACK\\ARM\\CMSIS\\" + cmsisVer + "\\CMSIS\\Include;";
	include += keilInstaPath + "\\ARM\\PACK\\Keil\\" + devPackName + "\\" + devPackVer + "\\Device\\StdPeriph_Driver\\inc;";
	include += keilInstaPath + "\\ARM\\PACK\\Keil\\" + devPackName + "\\" + devPackVer + "\\Device\\Include;";

	tinyxml2::XMLElement *xmlTemp;
	xmlTemp = projDoc.FirstChildElement("Project")->FirstChildElement("Targets")->FirstChildElement("Target")
		->FirstChildElement("TargetOption")->FirstChildElement("TargetArmAds")->FirstChildElement("Cads")
		->FirstChildElement("VariousControls")->FirstChildElement("Define");
	if (xmlTemp->GetText() != NULL)
		define = xmlTemp->GetText();
	for (int i = 0; i < define.size(); i++)
	{
		if (define[i] == ',')
			define[i] = ';';
	}

	cout << "生成VisualStudio2015解决方案" << endl;
	createSln(vsSlnPath + "\\" + projName + ".sln", projName);
	createKeilProjectProps(vsProjPath + "\\Keil5Project.props", include, define);
	createVcxproj(vsProjPath + "\\" + projName + ".vcxproj", projName, itemGroup);
	createFilters(vsProjPath + "\\" + projName + ".vcxproj.filters", itemGroup);
	createUser(vsProjPath + "\\" + projName + ".vcxproj.user");
}
