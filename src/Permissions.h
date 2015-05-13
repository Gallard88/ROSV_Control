#ifndef __PERMISSION_H__
#define __PERMISSION_H__
#include <string>
#include <memory>
#include <vector>
#include <atomic>

/*	Permission Class
 *	An object to control system wide modules.
 *	I.E. Enable motors when a client connects, disable when it disconnects.
 *	Can be used as boolean logic (On/Off), or as a counter ( On = non-zero value)
 *
 */

class Permission
{
public:
  Permission(std::string name );
  virtual ~Permission();

  bool isEnabled(void) const;
  void Set(bool en);
  std::string GetName() const;

private:
  std::string Name;
  std::atomic_bool Value;

  // Disable copying.
  Permission(const Permission &);
  Permission & operator=(const Permission &);
};

class PermissionGroup
{
public:
  PermissionGroup();
  virtual ~PermissionGroup();

  void add(std::shared_ptr<const Permission > p);
  void add(const PermissionGroup & p);
  void SetName(std::string name);

  bool isGroupEnabled();  // returns true if all member Permissions are enabled.

protected:
  std::vector<std::shared_ptr<const Permission >> PermList;
  std::string Name;
  std::atomic_bool GroupState;

  // Disable copying
  PermissionGroup & operator=(const PermissionGroup&);
  PermissionGroup & operator+(const PermissionGroup&);
};

#endif
