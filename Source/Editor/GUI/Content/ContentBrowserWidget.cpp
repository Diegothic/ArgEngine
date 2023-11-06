#include "ContentBrowserWidget.h"

#include "Resources/Content.h"


Arg::ContentBrowserWidget::ContentBrowserWidget(Vec2 position, Vec2 size, Content* content)
	: EditorWidget(position, size),
	m_pContent(content)
{
	m_SelectedDirectory = content->GetRootDirectory();
}

void Arg::ContentBrowserWidget::VOnDraw()
{
	const Vec2 position = GetPosition();
	const Vec2 size = GetSize();

	ImGui::BeginChild("##Directories", ImVec2(size.x * 0.3f, size.y - 45.0f), true, ImGuiWindowFlags_NoTitleBar);

	if (ImGui::CollapsingHeader("Content##Directories", ImGuiTreeNodeFlags_DefaultOpen))
	{
		const std::filesystem::path rootDirectory = m_pContent->GetRootDirectory();
		for (const auto& entry : std::filesystem::directory_iterator(rootDirectory))
		{
			if (std::filesystem::is_directory(entry.path()))
			{
				DrawDirectory(entry.path());
			}
		}
	}

	ImGui::EndChild();
	ImGui::SameLine();
	ImGui::BeginChild("##Files", ImVec2(size.x * 0.68f, size.y - 45.0f), true, ImGuiWindowFlags_NoTitleBar);

	if (!m_HasSelectedDirectory)
	{
		DrawContents(m_pContent->GetRootDirectory());
	}
	else
	{
		DrawContents(m_SelectedDirectory);
	}

	ImGui::EndChild();
}

void Arg::ContentBrowserWidget::VOnResized(Vec2 newPosition, Vec2 newSize)
{
	EditorWidget::VOnResized(newPosition, newSize);
}

void Arg::ContentBrowserWidget::DrawDirectory(const std::filesystem::path& path)
{
	const std::string directoryName = path.filename().string();
	ImGui::PushID(directoryName.c_str());

	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_OpenOnDoubleClick
		| ImGuiTreeNodeFlags_FramePadding;

	if (m_HasSelectedDirectory && path == m_SelectedDirectory)
	{
		nodeFlags |= ImGuiTreeNodeFlags_Selected;
	}

	bool hasSubDirectory = false;
	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (std::filesystem::is_directory(entry.path()))
		{
			hasSubDirectory = true;
			break;
		}
	}

	if (!hasSubDirectory)
	{
		nodeFlags |= ImGuiTreeNodeFlags_Leaf;
	}

	const bool isOpen = ImGui::TreeNodeEx(directoryName.c_str(), nodeFlags);
	if (ImGui::IsItemClicked())
	{
		m_HasSelectedDirectory = true;
		m_SelectedDirectory = path;
		m_HasSelectedFile = false;
	}

	if (isOpen)
	{
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			if (std::filesystem::is_directory(entry.path()))
			{
				DrawDirectory(entry.path());
			}
		}

		ImGui::TreePop();
	}

	ImGui::PopID();
}

void Arg::ContentBrowserWidget::DrawContents(const std::filesystem::path& path)
{
	const std::string directoryName = path.filename().string();
	ImGui::PushID(directoryName.c_str());

	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf
			| ImGuiTreeNodeFlags_FramePadding;

		if (m_HasSelectedFile && entry.path().filename() == m_SelectedFile)
		{
			nodeFlags |= ImGuiTreeNodeFlags_Selected;
		}

		bool isOpen = false;
		if (std::filesystem::is_directory(entry.path()))
		{
			isOpen = ImGui::TreeNodeEx(entry.path().filename().string().c_str(), nodeFlags);
			if (ImGui::IsItemClicked())
			{
				m_HasSelectedFile = true;
				m_SelectedFile = entry.path().filename();
			}

			if (ImGui::IsItemHovered()
				&& ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				m_HasSelectedDirectory = true;
				m_SelectedDirectory = entry.path();
				m_HasSelectedFile = false;
			}
		}
		else if (std::filesystem::is_regular_file(entry.path()) && entry.path().extension() == ".rsrc")
		{
			std::string fileName = entry.path().filename().replace_extension().string();
			isOpen = ImGui::TreeNodeEx(fileName.c_str(), nodeFlags);
			if (ImGui::IsItemClicked())
			{
				m_HasSelectedFile = true;
				m_SelectedFile = entry.path().filename();
			}

			const YAML::Node resource = YAML::LoadFile(entry.path().string());
			if (const YAML::Node header = resource["resource"])
			{
				const auto id = header["id"].as<uint64_t>();
				const auto type = header["type"].as<std::string>();
				const auto file = header["file"].as<std::string>();

				if (type == "Texture")
				{
					GUID resourceID = GUID(id);
					if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("TextureResourceID", &resourceID, sizeof(GUID));
						ImGui::Text(fileName.c_str());
						ImGui::EndDragDropSource();
					}
				}
			}
		}

		if (isOpen)
		{
			ImGui::TreePop();
		}
	}

	ImGui::PopID();
}
