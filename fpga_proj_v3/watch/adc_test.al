<?xml version="1.0" encoding="UTF-8"?>
<Project Version="3" Minor="2" Path="D:/05-work/demo_code/test_xintf/fpga_proj_v3/watch">
    <Project_Created_Time></Project_Created_Time>
    <TD_Encoding>UTF-8</TD_Encoding>
    <TD_Version>5.6.56362</TD_Version>
    <Name>adc_test</Name>
    <HardWare>
        <Family>PH1</Family>
        <Device>PH1A60GEG324</Device>
        <Speed></Speed>
    </HardWare>
    <Source_Files>
        <ADC_FILE>
            <File Path="constrain/io.adc">
                <FileInfo>
                    <Attr Name="UsedInSyn" Val="true"/>
                    <Attr Name="UsedInP&R" Val="true"/>
                    <Attr Name="BelongTo" Val="constraint_1"/>
                    <Attr Name="CompileOrder" Val="1"/>
                </FileInfo>
            </File>
        </ADC_FILE>
        <SDC_FILE>
            <File Path="constrain/timing.sdc">
                <FileInfo>
                    <Attr Name="UsedInSyn" Val="true"/>
                    <Attr Name="UsedInP&R" Val="true"/>
                    <Attr Name="BelongTo" Val="constraint_1"/>
                    <Attr Name="CompileOrder" Val="2"/>
                </FileInfo>
            </File>
        </SDC_FILE>
        <CWC_FILE>
            <File Path="watch.cwc">
                <FileInfo>
                    <Attr Name="UsedInSyn" Val="true"/>
                    <Attr Name="UsedInP&R" Val="true"/>
                    <Attr Name="BelongTo" Val="design_1"/>
                    <Attr Name="CompileOrder" Val="1"/>
                </FileInfo>
            </File>
        </CWC_FILE>
    </Source_Files>
    <FileSets>
        <FileSet Name="design_1" Type="DesignFiles">
        </FileSet>
        <FileSet Name="constraint_1" Type="ConstrainFiles">
        </FileSet>
    </FileSets>
    <TOP_MODULE>
        <LABEL></LABEL>
        <MODULE>top</MODULE>
        <CREATEINDEX>user</CREATEINDEX>
    </TOP_MODULE>
    <Property>
        <SimProperty>
            <lib>D:/04-software/modelsim10.4/examples/PH1</lib>
        </SimProperty>
    </Property>
    <Device_Settings>
    </Device_Settings>
    <Configurations>
        <Control0>
            <mclk_freq>33MHz</mclk_freq>
        </Control0>
        <FeatureRow>
            <boot_mode>mspix4</boot_mode>
        </FeatureRow>
    </Configurations>
    <Runs>
        <Run Name="syn_1" Type="Synthesis" ConstraintSet="constraint_1" Description="" Active="true">
            <Strategy Name="Default_Synthesis_Strategy">
            </Strategy>
        </Run>
        <Run Name="phy_1" Type="PhysicalDesign" ConstraintSet="constraint_1" Description="" SynRun="syn_1" Active="true">
            <Strategy Name="Default_PhysicalDesign_Strategy">
                <BitgenProperty::GeneralOption>
                    <bin>on</bin>
                </BitgenProperty::GeneralOption>
            </Strategy>
        </Run>
    </Runs>
    <Project_Settings>
    </Project_Settings>
</Project>
