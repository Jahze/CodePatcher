#ifndef PATCH_H_
#define PATCH_H_

#include <string>

class Patch {
public:
	Patch(const std::string& name, const std::string& desc, void *pAddress, const char *bytes, unsigned length);
	~Patch();

	std::string GetName() const;
	std::string GetDescription() const;
	void *GetAddress() const;
	unsigned GetLength() const;

	bool IsPatched() const;
	bool ApplyPatch();
	void Unpatch();

private:
	std::string m_name;
	std::string m_description;
	void *m_pAddress;
	char *m_pPatchBytes;
	char *m_pOldBytes;
	unsigned m_length;
	bool m_bPatched;
};

#endif
