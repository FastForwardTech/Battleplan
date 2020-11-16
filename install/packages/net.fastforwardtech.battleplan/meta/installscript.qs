function Component()
{
    installer.gainAdminRights();
    component.loaded.connect(this, this.installerLoaded);
}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut", "@TargetDir@/Battleplan.exe", "@StartMenuDir@/Battleplan.lnk",
                                "workingDirectory=@TargetDir@", "iconPath=%SystemRoot%/system32/SHELL32.dll",
                                "iconId=2", "description=Run Battleplan");
    }
}

Component.prototype.installerLoaded = function()
{
    gui.pageById(QInstaller.TargetDirectory).entered.connect(this, this.targetDirectoryPageEntered);
}

Component.prototype.targetDirectoryPageEntered = function()
{
    var dir = installer.value("TargetDir");
    if (installer.fileExists(dir) && installer.fileExists(dir + "/maintenancetool.exe")) {
        installer.execute(dir + "/maintenancetool.exe", "--script=" + dir + "/scripts/auto_uninstall.qs");
    }
}
