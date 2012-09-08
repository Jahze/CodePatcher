#include "commands.h"
#include "Patch.h"
#include "PatchManager.h"

namespace {
	void ListCommandCallback(const CCommand &command) {
		PatchCollection &patches = g_PatchManager.GetAllPatches();

		g_pSM->LogMessage(myself, "Name\tDescription\tAddress\tLength\tApplied");
		for (PatchCollection::iterator iter = patches.begin(); iter != patches.end(); ++iter) {
			g_pSM->LogMessage(myself, "%s\t%s\t%p\t%d\t%s",
				(*iter)->GetName().c_str(),
				(*iter)->GetDescription().c_str(),
				(*iter)->GetAddress(),
				(*iter)->GetLength(),
				(*iter)->IsPatched() ? "Yes" : "No");
		}
	}

	Patch *FindPatchByName(const CCommand &command) {
		if (command.ArgC() < 2) {
			g_pSM->LogError(myself, "Syntax: %s <name>", command.Arg(0));
			return NULL;
		}

		Patch *patch = g_PatchManager.GetPatchByName(command.Arg(1));
		if (patch == NULL) {
			g_pSM->LogError(myself, "Could not find patch \"%s\"", command.Arg(1));
			return NULL;
		}

		return patch;
	}

	void PatchCommandCallback(const CCommand &command) {
		Patch *patch = FindPatchByName(command);
		
		if (patch == NULL)
			return;

		patch->ApplyPatch();
	}

	void UnpatchCommandCallback(const CCommand &command) {
		Patch *patch = FindPatchByName(command);
		
		if (patch == NULL)
			return;

		patch->Unpatch();
	}
}

ListCommand::ListCommand()
	: ConCommand("sm_codepatch_list", ::ListCommandCallback, "Lists available code patches and their status") {
}

PatchCommand::PatchCommand()
	: ConCommand("sm_codepatch_patch", ::PatchCommandCallback, "Applies a code patch") {
}

UnpatchCommand::UnpatchCommand()
	: ConCommand("sm_codepatch_unpatch", ::UnpatchCommandCallback, "Unloads a code patch") {
}
