function Component()
{
}

Component.prototype.isDefault = function()
{
	return true;
}

Component.prototype.createOperations = function()
{
    component.createOperations();
    if (systemInfo.productType === "windows") 
	{
        component.addOperation("CreateShortcut", "@TargetDir@/Firelite.exe", "@StartMenuDir@/Firelite.lnk",
            "workingDirectory=@TargetDir@");
			
		component.addOperation("CreateShortcut", "@TargetDir@/Firelite.exe", "@DesktopDir@/Firelite.lnk",
            "workingDirectory=@TargetDir@");
    }
}
