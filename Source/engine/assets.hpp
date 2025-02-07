#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <optional>
#include <string>
#include <string_view>

#include <SDL.h>
#include <expected.hpp>

#include <fmt/format.h>

#include "appfat.h"
#include "game_mode.hpp"
#include "headless_mode.hpp"
#include "utils/file_util.h"
#include "utils/language.h"
#include "utils/str_cat.hpp"
#include "utils/string_or_view.hpp"

#ifndef UNPACKED_MPQS
#include "mpq/mpq_reader.hpp"
#endif

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

namespace devilution {

#ifdef UNPACKED_MPQS
struct AssetRef {
	static constexpr size_t PathBufSize = 4088;

	char path[PathBufSize];

	[[nodiscard]] bool ok() const
	{
		return path[0] != '\0';
	}

	// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
	[[nodiscard]] const char *error() const
	{
		return "File not found";
	}

	[[nodiscard]] size_t size() const
	{
		uintmax_t fileSize;
		if (!GetFileSize(path, &fileSize))
			return 0;
		return fileSize;
	}
};

struct AssetHandle {
	FILE *handle = nullptr;

	AssetHandle() = default;

	AssetHandle(FILE *handle)
	    : handle(handle)
	{
	}

	AssetHandle(AssetHandle &&other) noexcept
	    : handle(other.handle)
	{
		other.handle = nullptr;
	}

	AssetHandle &operator=(AssetHandle &&other) noexcept
	{
		handle = other.handle;
		other.handle = nullptr;
		return *this;
	}

	~AssetHandle()
	{
		if (handle != nullptr)
			std::fclose(handle);
	}

	[[nodiscard]] bool ok() const
	{
		return handle != nullptr && std::ferror(handle) == 0;
	}

	bool read(void *buffer, size_t len)
	{
		return std::fread(buffer, len, 1, handle) == 1;
	}

	bool seek(long pos)
	{
		return std::fseek(handle, pos, SEEK_SET) == 0;
	}

	[[nodiscard]] const char *error() const
	{
		return std::strerror(errno);
	}
};
#else
struct AssetRef {
	// An MPQ file reference:
	MpqArchive *archive = nullptr;
	uint32_t fileNumber;
	std::string_view filename;

	// Alternatively, a direct SDL_RWops handle:
	SDL_RWops *directHandle = nullptr;

	AssetRef() = default;

	AssetRef(AssetRef &&other) noexcept
	    : archive(other.archive)
	    , fileNumber(other.fileNumber)
	    , filename(other.filename)
	    , directHandle(other.directHandle)
	{
		other.directHandle = nullptr;
	}

	AssetRef &operator=(AssetRef &&other) noexcept
	{
		if (directHandle != nullptr)
			SDL_RWclose(directHandle);
		archive = other.archive;
		fileNumber = other.fileNumber;
		filename = other.filename;
		directHandle = other.directHandle;
		other.directHandle = nullptr;
		return *this;
	}

	~AssetRef()
	{
		if (directHandle != nullptr)
			SDL_RWclose(directHandle);
	}

	[[nodiscard]] bool ok() const
	{
		return directHandle != nullptr || archive != nullptr;
	}

	// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
	[[nodiscard]] const char *error() const
	{
		return SDL_GetError();
	}

	[[nodiscard]] size_t size() const
	{
		if (archive != nullptr) {
			int32_t error;
			return archive->GetUnpackedFileSize(fileNumber, error);
		}
		return static_cast<size_t>(SDL_RWsize(directHandle));
	}
};

struct AssetHandle {
	SDL_RWops *handle = nullptr;

	AssetHandle() = default;

	explicit AssetHandle(SDL_RWops *handle)
	    : handle(handle)
	{
	}

	AssetHandle(AssetHandle &&other) noexcept
	    : handle(other.handle)
	{
		other.handle = nullptr;
	}

	AssetHandle &operator=(AssetHandle &&other) noexcept
	{
		if (handle != nullptr) {
			SDL_RWclose(handle);
		}
		handle = other.handle;
		other.handle = nullptr;
		return *this;
	}

	~AssetHandle()
	{
		if (handle != nullptr)
			SDL_RWclose(handle);
	}

	[[nodiscard]] bool ok() const
	{
		return handle != nullptr;
	}

	bool read(void *buffer, size_t len)
	{
#if SDL_VERSION_ATLEAST(2, 0, 0)
		return handle->read(handle, buffer, len, 1) == 1;
#else
		return handle->read(handle, buffer, static_cast<int>(len), 1) == 1;
#endif
	}

	bool seek(long pos)
	{
		return handle->seek(handle, pos, RW_SEEK_SET) != -1;
	}

	[[nodiscard]] const char *error() const
	{
		return SDL_GetError();
	}

	SDL_RWops *release() &&
	{
		SDL_RWops *result = handle;
		handle = nullptr;
		return result;
	}
};
#endif

std::string FailedToOpenFileErrorMessage(std::string_view path, std::string_view error);

[[noreturn]] inline void FailedToOpenFileError(std::string_view path, std::string_view error)
{
	app_fatal(FailedToOpenFileErrorMessage(path, error));
}

inline bool ValidatAssetRef(std::string_view path, const AssetRef &ref)
{
	if (ref.ok())
		return true;
	if (!HeadlessMode) {
		FailedToOpenFileError(path, ref.error());
	}
	return false;
}

inline bool ValidateHandle(std::string_view path, const AssetHandle &handle)
{
	if (handle.ok())
		return true;
	if (!HeadlessMode) {
		FailedToOpenFileError(path, handle.error());
	}
	return false;
}

AssetRef FindAsset(std::string_view filename);

AssetHandle OpenAsset(AssetRef &&ref, bool threadsafe = false);
AssetHandle OpenAsset(std::string_view filename, bool threadsafe = false);
AssetHandle OpenAsset(std::string_view filename, size_t &fileSize, bool threadsafe = false);

SDL_RWops *OpenAssetAsSdlRwOps(std::string_view filename, bool threadsafe = false);

struct AssetData {
	std::unique_ptr<char[]> data;
	size_t size;

	explicit operator std::string_view() const
	{
		return std::string_view(data.get(), size);
	}
};

tl::expected<AssetData, std::string> LoadAsset(std::string_view path);

#ifdef UNPACKED_MPQS
extern DVL_API_FOR_TEST std::optional<std::string> spawn_data_path;
extern DVL_API_FOR_TEST std::optional<std::string> diabdat_data_path;
extern std::optional<std::string> hellfire_data_path;
extern std::optional<std::string> font_data_path;
extern std::optional<std::string> lang_data_path;
#else
/** A handle to the spawn.mpq archive. */
extern DVL_API_FOR_TEST std::optional<MpqArchive> spawn_mpq;
/** A handle to the diabdat.mpq archive. */
extern DVL_API_FOR_TEST std::optional<MpqArchive> diabdat_mpq;
/** A handle to an hellfire.mpq archive. */
extern std::optional<MpqArchive> hellfire_mpq;
extern std::optional<MpqArchive> hfmonk_mpq;
extern std::optional<MpqArchive> hfbard_mpq;
extern std::optional<MpqArchive> hfbarb_mpq;
extern std::optional<MpqArchive> hfmusic_mpq;
extern std::optional<MpqArchive> hfvoice_mpq;
extern std::optional<MpqArchive> font_mpq;
extern std::optional<MpqArchive> lang_mpq;
extern std::optional<MpqArchive> devilutionx_mpq;
#endif

void LoadCoreArchives();
void LoadLanguageArchive();
void LoadGameArchives();

#ifdef UNPACKED_MPQS
[[nodiscard]] inline bool HaveSpawn() { return spawn_data_path.has_value(); }
[[nodiscard]] inline bool HaveDiabdat() { return diabdat_data_path.has_value(); }
[[nodiscard]] inline bool HaveHellfire() { return hellfire_data_path.has_value(); }
[[nodiscard]] inline bool HaveExtraFonts() { return font_data_path.has_value(); }

// Bard and barbarian are not currently supported in unpacked mode.
[[nodiscard]] inline bool HaveBardAssets() { return false; }
[[nodiscard]] inline bool HaveBarbarianAssets() { return false; }
#else
[[nodiscard]] inline bool HaveSpawn() { return spawn_mpq.has_value(); }
[[nodiscard]] inline bool HaveDiabdat() { return diabdat_mpq.has_value(); }
[[nodiscard]] inline bool HaveHellfire() { return hellfire_mpq.has_value(); }
[[nodiscard]] inline bool HaveExtraFonts() { return font_mpq.has_value(); }
[[nodiscard]] inline bool HaveBardAssets() { return hfbard_mpq.has_value(); }
[[nodiscard]] inline bool HaveBarbarianAssets() { return hfbarb_mpq.has_value(); }
#endif

} // namespace devilution
