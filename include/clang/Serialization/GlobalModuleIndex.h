//===--- GlobalModuleIndex.h - Global Module Index --------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the GlobalModuleIndex class, which manages a global index
// containing all of the identifiers with namespace-scope bindings attached to
// them as well as all of the selectors that name methods, across all of the
// modules within a given subdirectory of the module cache. It is used to
// improve the performance of queries such as "does this identifier have any
// top-level bindings in any module?"
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_CLANG_SERIALIZATION_GLOBAL_MODULE_INDEX_H
#define LLVM_CLANG_SERIALIZATION_GLOBAL_MODULE_INDEX_H

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/OwningPtr.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include <utility>

namespace llvm {
class BitstreamCursor;
class MemoryBuffer;
}

namespace clang {

class DirectoryEntry;
class FileEntry;
class FileManager;

using llvm::SmallVector;
using llvm::SmallVectorImpl;
using llvm::StringRef;

/// \brief A global index for a set of module files, providing information about
/// the top-level identifiers and selectors within those module files.
///
/// The global index is an aid for name lookup into modules, offering a central
/// place where one can look for identifiers or selectors to determine which
/// module files contain a namespace-scope identity with that identifier or
/// a method with that selector, respectively. This allows the client to
/// restrict the search to only those module files known to have a binding for
/// that identifier or selector, improving performance. Moreover, the global
/// module index may know about module files that have not been imported, and
/// can be queried to determine which modules the currently translation could
/// or should load to fix a problem.
class GlobalModuleIndex {
  /// \brief Buffer containing the index file, which is lazily accessed so long
  /// as the global module index is live.
  llvm::OwningPtr<llvm::MemoryBuffer> Buffer;

  /// \brief The hash table.
  ///
  /// This pointer actually points to a IdentifierIndexTable object,
  /// but that type is only accessible within the implementation of
  /// GlobalModuleIndex.
  void *IdentifierIndex;

  /// \brief Information about a given module file.
  struct ModuleInfo {
    ModuleInfo() : File() { }

    /// \brief The module file entry.
    const FileEntry *File;

    /// \brief The module files on which this module directly depends.
    llvm::SmallVector<const FileEntry *, 4> Dependencies;
  };

  /// \brief A mapping from module IDs to information about each module.
  ///
  /// This vector may have gaps, if module files have been removed or have
  /// been updated since the index was built. A gap is indicated by an empty
  /// \c File pointer.
  llvm::SmallVector<ModuleInfo, 16> Modules;

  /// \brief Lazily-populated mapping from module file entries to their
  /// corresponding index into the \c Modules vector.
  llvm::DenseMap<const FileEntry *, unsigned> ModulesByFile;

  /// \brief The number of identifier lookups we performed.
  unsigned NumIdentifierLookups;

  /// \brief The number of identifier lookup hits, where we recognize the
  /// identifier.
  unsigned NumIdentifierLookupHits;

  /// \brief Internal constructor. Use \c readIndex() to read an index.
  explicit GlobalModuleIndex(FileManager &FileMgr, llvm::MemoryBuffer *Buffer,
                             llvm::BitstreamCursor Cursor);

  GlobalModuleIndex(const GlobalModuleIndex &); // DO NOT IMPLEMENT
  GlobalModuleIndex &operator=(const GlobalModuleIndex &); // DO NOT IMPLEMENT

public:
  ~GlobalModuleIndex();

  /// \brief An error code returned when trying to read an index.
  enum ErrorCode {
    /// \brief No error occurred.
    EC_None,
    /// \brief No index was found.
    EC_NotFound,
    /// \brief Some other process is currently building the index; it is not
    /// available yet.
    EC_Building,
    /// \brief There was an unspecified I/O error reading or writing the index.
    EC_IOError
  };

  /// \brief Read a global index file for the given directory.
  ///
  /// \param FileMgr The file manager to use for reading files.
  ///
  /// \param Path The path to the specific module cache where the module files
  /// for the intended configuration reside.
  ///
  /// \returns A pair containing the global module index (if it exists) and
  /// the error code.
  static std::pair<GlobalModuleIndex *, ErrorCode>
  readIndex(FileManager &FileMgr, StringRef Path);

  /// \brief Retrieve the set of modules that have up-to-date indexes.
  ///
  /// \param ModuleFiles Will be populated with the set of module files that
  /// have been indexed.
  void getKnownModules(SmallVectorImpl<const FileEntry *> &ModuleFiles);

  /// \brief Retrieve the set of module files on which the given module file
  /// directly depends.
  void getModuleDependencies(const FileEntry *ModuleFile,
                             SmallVectorImpl<const FileEntry *> &Dependencies);

  /// \brief A set of module files in which we found a result.
  typedef llvm::SmallPtrSet<const FileEntry *, 4> HitSet;
  
  /// \brief Look for all of the module files with a namespace-scope binding
  /// for the given identifier, e.g., a global function, variable, or type with
  /// that name, or declare a method with the selector.
  ///
  /// \param Name The identifier to look for.
  ///
  /// \param Hits Will be populated with the set of module
  /// files that declare entities with the given name.
  ///
  /// \returns true if the identifier is known to the index, false otherwise.
  bool lookupIdentifier(StringRef Name, HitSet &Hits);

  /// \brief Print statistics to standard error.
  void printStats();

  /// \brief Write a global index into the given
  ///
  /// \param FileMgr The file manager to use to load module files.
  ///
  /// \param Path The path to the directory containing module files, into
  /// which the global index will be written.
  static ErrorCode writeIndex(FileManager &FileMgr, StringRef Path);
};

}

#endif
