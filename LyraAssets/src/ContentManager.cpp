#include "ContentManager.h"

ContentManager::ContentManager() : m_currentAssetFile(lyra::assetsFilePath(), lyra::OpenMode::readExtText) {
	lyra::StringStream recentsFile("data/recents.dat", lyra::OpenMode::readExtText);

	if (!recentsFile.good()) {
		recentsFile = lyra::StringStream("data/recents.dat", lyra::OpenMode::writeExtText);
	} else {
		lyra::uint32 i = 0;
		bool finishedParsing = false;

		// parse the recents file
		while (!finishedParsing) {
			m_recents.resize(i);

			std::string buf;
			
			int c = recentsFile.get();
			while (c != '\n') {
				buf.push_back(c);
				c = recentsFile.get();
			}

			m_recents[i] = buf;
			buf.clear();

			bool skippedBlanks = false;

			while (!skippedBlanks) {
				c = recentsFile.get();

				switch (c) {
					case ' ':
					case '\n':
					case '\t':
					case '\r':
					case '\0':
						break;

					case static_cast<int>(lyra::FileState::eof):
						finishedParsing = true;
					
					default:
						skippedBlanks = true;
						break;
				}
			}
			
			++i;
		}
	}

	if (m_currentAssetFile.good()) {
		m_validProject = true;
	}
}

void ContentManager::loadProjectFile() {

}

void ContentManager::createProjectFile() {

}

void ContentManager::save() {

}

void ContentManager::saveAs() {

}

void ContentManager::createItem() {

}

void ContentManager::createFolder() {

}

void ContentManager::loadItem() {

}

void ContentManager::loadFolder() {

}

void ContentManager::build() {

}

void ContentManager::rebuild() {

}

void ContentManager::clean() {

}

void ContentManager::cancel() {

}
