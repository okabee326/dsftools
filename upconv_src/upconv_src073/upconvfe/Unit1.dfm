object Form1: TForm1
  Left = 311
  Top = 115
  Caption = 'Upconv Frontend 0.7.x'
  ClientHeight = 680
  ClientWidth = 576
  Color = cl3DLight
  Constraints.MaxHeight = 707
  Constraints.MaxWidth = 584
  Constraints.MinHeight = 311
  Constraints.MinWidth = 584
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'FixedSys'
  Font.Style = []
  OldCreateOrder = False
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 15
  object StatusBar1: TStatusBar
    Left = 0
    Top = 661
    Width = 576
    Height = 19
    Panels = <>
    SimplePanel = True
  end
  object PageControl1: TPageControl
    Left = 0
    Top = 0
    Width = 576
    Height = 661
    ActivePage = TabSheet3
    Align = alClient
    TabOrder = 1
    object TabSheet3: TTabSheet
      Caption = 'Upconv'
      ImageIndex = 2
      object CategoryPanelGroup2: TCategoryPanelGroup
        Left = 0
        Top = 0
        Width = 568
        Height = 631
        VertScrollBar.Tracking = True
        Align = alClient
        HeaderFont.Charset = DEFAULT_CHARSET
        HeaderFont.Color = clWindowText
        HeaderFont.Height = -11
        HeaderFont.Name = 'Tahoma'
        HeaderFont.Style = []
        TabOrder = 0
        object CategoryPanel8: TCategoryPanel
          Top = 491
          Height = 136
          Caption = 'Control'
          TabOrder = 0
          OnCollapse = CategoryPanel8Collapse
          object lblFilename: TLabel
            Left = 21
            Top = 10
            Width = 511
            Height = 18
            AutoSize = False
            Color = clWhite
            ParentColor = False
          end
          object lblProcess1: TLabel
            Left = 34
            Top = 36
            Width = 135
            Height = 15
            AutoSize = False
            Layout = tlCenter
          end
          object ProgressBar1: TProgressBar
            Left = 179
            Top = 34
            Width = 353
            Height = 17
            Smooth = True
            Step = 1
            TabOrder = 0
          end
          object btnAbort: TBitBtn
            Left = 358
            Top = 75
            Width = 75
            Height = 25
            Caption = '&Abort'
            DoubleBuffered = True
            Glyph.Data = {
              DE010000424DDE01000000000000760000002800000024000000120000000100
              0400000000006801000000000000000000001000000000000000000000000000
              80000080000000808000800000008000800080800000C0C0C000808080000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
              333333333333333333333333000033338833333333333333333F333333333333
              0000333911833333983333333388F333333F3333000033391118333911833333
              38F38F333F88F33300003339111183911118333338F338F3F8338F3300003333
              911118111118333338F3338F833338F3000033333911111111833333338F3338
              3333F8330000333333911111183333333338F333333F83330000333333311111
              8333333333338F3333383333000033333339111183333333333338F333833333
              00003333339111118333333333333833338F3333000033333911181118333333
              33338333338F333300003333911183911183333333383338F338F33300003333
              9118333911183333338F33838F338F33000033333913333391113333338FF833
              38F338F300003333333333333919333333388333338FFF830000333333333333
              3333333333333333333888330000333333333333333333333333333333333333
              0000}
            ModalResult = 3
            NumGlyphs = 2
            ParentDoubleBuffered = False
            TabOrder = 1
            Visible = False
            OnClick = btnAbortClick
          end
          object btnExit: TBitBtn
            Left = 457
            Top = 75
            Width = 75
            Height = 25
            Caption = 'E&xit'
            DoubleBuffered = True
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = []
            Glyph.Data = {
              DE010000424DDE01000000000000760000002800000024000000120000000100
              0400000000006801000000000000000000001000000000000000000000000000
              80000080000000808000800000008000800080800000C0C0C000808080000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00388888888877
              F7F787F8888888888333333F00004444400888FFF444448888888888F333FF8F
              000033334D5007FFF4333388888888883338888F0000333345D50FFFF4333333
              338F888F3338F33F000033334D5D0FFFF43333333388788F3338F33F00003333
              45D50FEFE4333333338F878F3338F33F000033334D5D0FFFF43333333388788F
              3338F33F0000333345D50FEFE4333333338F878F3338F33F000033334D5D0FFF
              F43333333388788F3338F33F0000333345D50FEFE4333333338F878F3338F33F
              000033334D5D0EFEF43333333388788F3338F33F0000333345D50FEFE4333333
              338F878F3338F33F000033334D5D0EFEF43333333388788F3338F33F00003333
              4444444444333333338F8F8FFFF8F33F00003333333333333333333333888888
              8888333F00003333330000003333333333333FFFFFF3333F00003333330AAAA0
              333333333333888888F3333F00003333330000003333333333338FFFF8F3333F
              0000}
            NumGlyphs = 2
            ParentDoubleBuffered = False
            ParentFont = False
            TabOrder = 2
            OnClick = btnExitClick
          end
          object btnStart: TBitBtn
            Left = 358
            Top = 75
            Width = 75
            Height = 25
            Caption = '&Start'
            DoubleBuffered = True
            Enabled = False
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = []
            Glyph.Data = {
              DE010000424DDE01000000000000760000002800000024000000120000000100
              0400000000006801000000000000000000001000000000000000000000000000
              80000080000000808000800000008000800080800000C0C0C000808080000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
              3333333333333333333333330000333333333333333333333333F33333333333
              00003333344333333333333333388F3333333333000033334224333333333333
              338338F3333333330000333422224333333333333833338F3333333300003342
              222224333333333383333338F3333333000034222A22224333333338F338F333
              8F33333300003222A3A2224333333338F3838F338F33333300003A2A333A2224
              33333338F83338F338F33333000033A33333A222433333338333338F338F3333
              0000333333333A222433333333333338F338F33300003333333333A222433333
              333333338F338F33000033333333333A222433333333333338F338F300003333
              33333333A222433333333333338F338F00003333333333333A22433333333333
              3338F38F000033333333333333A223333333333333338F830000333333333333
              333A333333333333333338330000333333333333333333333333333333333333
              0000}
            ModalResult = 1
            NumGlyphs = 2
            ParentDoubleBuffered = False
            ParentFont = False
            TabOrder = 3
            OnClick = btnStartClick
          end
        end
        object cpSetting: TCategoryPanel
          Top = 0
          Height = 491
          Caption = 'Setting'
          DoubleBuffered = True
          ParentDoubleBuffered = False
          TabOrder = 1
          OnCollapse = cpSettingCollapse
          object Label8: TLabel
            Left = 374
            Top = 199
            Width = 52
            Height = 15
            Caption = 'Total Size'
          end
          object Label28: TLabel
            Left = 7
            Top = 14
            Width = 26
            Height = 15
            Caption = 'Files'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Label29: TLabel
            Left = 7
            Top = 236
            Width = 44
            Height = 15
            Caption = 'Convert'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Bevel1: TBevel
            Left = 6
            Top = 223
            Width = 528
            Height = 8
            Shape = bsBottomLine
          end
          object Bevel17: TBevel
            Left = 6
            Top = 360
            Width = 528
            Height = 8
            Shape = bsBottomLine
          end
          object Label30: TLabel
            Left = 7
            Top = 374
            Width = 78
            Height = 15
            Caption = 'Output Option'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object shp2: TBevel
            Left = 198
            Top = 395
            Width = 334
            Height = 57
          end
          object Bevel12: TBevel
            Left = 6
            Top = 2
            Width = 528
            Height = 8
            Shape = bsBottomLine
          end
          object Label2: TLabel
            Left = 28
            Top = 422
            Width = 29
            Height = 15
            Caption = 'Suffix'
          end
          object shp1: TBevel
            Left = 15
            Top = 395
            Width = 177
            Height = 57
          end
          object Label16: TLabel
            Left = 30
            Top = 164
            Width = 53
            Height = 15
            Caption = 'Sampling'
          end
          object Label19: TLabel
            Left = 220
            Top = 166
            Width = 48
            Height = 15
            Caption = 'Bit Width'
          end
          object ListView: TListView
            Left = 21
            Top = 39
            Width = 517
            Height = 112
            Columns = <
              item
                Caption = 'Filename'
                MaxWidth = 250
                MinWidth = 250
                Width = 250
              end
              item
                Caption = 'Time'
                MaxWidth = 80
                MinWidth = 80
                Width = 80
              end
              item
                Caption = 'Size'
                MaxWidth = 80
                MinWidth = 80
                Width = 80
              end
              item
                Caption = 'Convtime'
                MaxWidth = 80
                MinWidth = 80
                Width = 80
              end>
            ColumnClick = False
            HideSelection = False
            LargeImages = ImageList
            ReadOnly = True
            RowSelect = True
            SmallImages = ImageList
            SortType = stText
            TabOrder = 0
            ViewStyle = vsReport
            OnChange = ListViewChange
            OnCustomDrawItem = ListViewCustomDrawItem
            OnSelectItem = ListViewSelectItem
          end
          object btnAdd: TBitBtn
            Left = 19
            Top = 192
            Width = 70
            Height = 25
            Caption = '&Add'
            DoubleBuffered = True
            Glyph.Data = {
              DE010000424DDE01000000000000760000002800000024000000120000000100
              0400000000006801000000000000000000001000000000000000000000000000
              80000080000000808000800000008000800080800000C0C0C000808080000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
              3333333333333333333333330000333333333333333333333333F33333333333
              00003333344333333333333333388F3333333333000033334224333333333333
              338338F3333333330000333422224333333333333833338F3333333300003342
              222224333333333383333338F3333333000034222A22224333333338F338F333
              8F33333300003222A3A2224333333338F3838F338F33333300003A2A333A2224
              33333338F83338F338F33333000033A33333A222433333338333338F338F3333
              0000333333333A222433333333333338F338F33300003333333333A222433333
              333333338F338F33000033333333333A222433333333333338F338F300003333
              33333333A222433333333333338F338F00003333333333333A22433333333333
              3338F38F000033333333333333A223333333333333338F830000333333333333
              333A333333333333333338330000333333333333333333333333333333333333
              0000}
            ModalResult = 1
            NumGlyphs = 2
            ParentDoubleBuffered = False
            TabOrder = 1
            OnClick = btnAddClick
          end
          object btnDel: TBitBtn
            Left = 95
            Top = 191
            Width = 70
            Height = 25
            Caption = '&Del'
            DoubleBuffered = True
            Enabled = False
            Glyph.Data = {
              DE010000424DDE01000000000000760000002800000024000000120000000100
              0400000000006801000000000000000000001000000000000000000000000000
              80000080000000808000800000008000800080800000C0C0C000808080000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
              333333333333333333333333000033338833333333333333333F333333333333
              0000333911833333983333333388F333333F3333000033391118333911833333
              38F38F333F88F33300003339111183911118333338F338F3F8338F3300003333
              911118111118333338F3338F833338F3000033333911111111833333338F3338
              3333F8330000333333911111183333333338F333333F83330000333333311111
              8333333333338F3333383333000033333339111183333333333338F333833333
              00003333339111118333333333333833338F3333000033333911181118333333
              33338333338F333300003333911183911183333333383338F338F33300003333
              9118333911183333338F33838F338F33000033333913333391113333338FF833
              38F338F300003333333333333919333333388333338FFF830000333333333333
              3333333333333333333888330000333333333333333333333333333333333333
              0000}
            ModalResult = 2
            NumGlyphs = 2
            ParentDoubleBuffered = False
            TabOrder = 2
            OnClick = btnDelClick
          end
          object btnClear: TBitBtn
            Left = 171
            Top = 191
            Width = 71
            Height = 25
            Caption = '&Clear'
            DoubleBuffered = True
            Enabled = False
            Glyph.Data = {
              DE010000424DDE01000000000000760000002800000024000000120000000100
              0400000000006801000000000000000000001000000000000000000000000000
              80000080000000808000800000008000800080800000C0C0C000808080000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
              333333333333333333333333000033338833333333333333333F333333333333
              0000333911833333983333333388F333333F3333000033391118333911833333
              38F38F333F88F33300003339111183911118333338F338F3F8338F3300003333
              911118111118333338F3338F833338F3000033333911111111833333338F3338
              3333F8330000333333911111183333333338F333333F83330000333333311111
              8333333333338F3333383333000033333339111183333333333338F333833333
              00003333339111118333333333333833338F3333000033333911181118333333
              33338333338F333300003333911183911183333333383338F338F33300003333
              9118333911183333338F33838F338F33000033333913333391113333338FF833
              38F338F300003333333333333919333333388333338FFF830000333333333333
              3333333333333333333888330000333333333333333333333333333333333333
              0000}
            ModalResult = 3
            NumGlyphs = 2
            ParentDoubleBuffered = False
            TabOrder = 3
            OnClick = btnClearClick
          end
          object btnUp: TButton
            Left = 260
            Top = 192
            Width = 28
            Height = 25
            Caption = #9650
            Enabled = False
            Font.Charset = ANSI_CHARSET
            Font.Color = clNavy
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = []
            ParentFont = False
            TabOrder = 4
            OnClick = btnUpClick
          end
          object btnDown: TButton
            Left = 296
            Top = 192
            Width = 28
            Height = 25
            Caption = #9660
            Enabled = False
            TabOrder = 5
            OnClick = btnDownClick
          end
          object btnInfo: TBitBtn
            Left = 337
            Top = 191
            Width = 27
            Height = 25
            Caption = #65281
            DoubleBuffered = True
            Enabled = False
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlue
            Font.Height = -24
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentDoubleBuffered = False
            ParentFont = False
            TabOrder = 6
            OnClick = btnInfoClick
          end
          object leTotalSize: TEdit
            Left = 443
            Top = 192
            Width = 89
            Height = 23
            Color = clMoneyGreen
            ReadOnly = True
            TabOrder = 7
          end
          object GroupBox2: TGroupBox
            Left = 15
            Top = 257
            Width = 100
            Height = 49
            Caption = 'Sampling rate'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 8
            object cmbSampRate: TComboBox
              Left = 10
              Top = 18
              Width = 79
              Height = 22
              Style = csOwnerDrawFixed
              ItemIndex = 0
              TabOrder = 0
              Text = '32000'
              OnChange = cmbSampRateChange
              Items.Strings = (
                '32000'
                '44100'
                '48000'
                '88200'
                '96000'
                '176400'
                '192000'
                '352800'
                '384000')
            end
          end
          object GroupBox3: TGroupBox
            Left = 126
            Top = 257
            Width = 75
            Height = 49
            Caption = 'Bit width'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 9
            object cmbBitwidth: TComboBox
              Left = 8
              Top = 16
              Width = 55
              Height = 22
              Style = csOwnerDrawFixed
              ItemIndex = 0
              TabOrder = 0
              Text = '16'
              OnChange = cmbBitwidthChange
              Items.Strings = (
                '16'
                '24'
                '32(Float)'
                '64(Float)')
            end
          end
          object GroupBox5: TGroupBox
            Left = 211
            Top = 257
            Width = 96
            Height = 49
            Hint = '|Specify cutoff frequency of a high level|2'
            CustomHint = BalloonHint
            Caption = 'HFC'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            ParentShowHint = False
            ShowHint = True
            TabOrder = 10
            object edtHFC: TEdit
              Left = 8
              Top = 16
              Width = 77
              Height = 23
              CustomHint = BalloonHint
              MaxLength = 6
              NumbersOnly = True
              ParentShowHint = False
              ShowHint = False
              TabOrder = 0
            end
          end
          object GroupBox6: TGroupBox
            Left = 317
            Top = 257
            Width = 97
            Height = 49
            Hint = '|Specify cutoff frequency of a low level|2'
            CustomHint = BalloonHint
            Caption = 'LFC'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            ParentShowHint = False
            ShowHint = True
            TabOrder = 11
            object edtLFC: TEdit
              Left = 8
              Top = 16
              Width = 77
              Height = 23
              CustomHint = BalloonHint
              MaxLength = 6
              NumbersOnly = True
              TabOrder = 0
            end
          end
          object GroupBox4: TGroupBox
            Left = 15
            Top = 310
            Width = 186
            Height = 49
            Caption = 'HFA'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 12
            object cmbHFA: TComboBox
              Left = 10
              Top = 18
              Width = 163
              Height = 22
              Style = csOwnerDrawFixed
              Font.Charset = ANSI_CHARSET
              Font.Color = clWindowText
              Font.Height = -12
              Font.Name = 'FixedSys'
              Font.Style = []
              ItemIndex = 0
              ParentFont = False
              TabOrder = 0
              Text = 'Cutoff(hfa0)'
              Items.Strings = (
                'Cutoff(hfa0)'
                'Noise(hfa1)'
                'OverTone(hfa2)'
                'OverTone Ex(hfa3)')
            end
          end
          object rdoToFileSrc: TRadioButton
            Left = 25
            Top = 402
            Width = 113
            Height = 17
            Caption = 'Source Directory'
            Checked = True
            TabOrder = 13
            TabStop = True
            OnClick = rdoToFileSrcClick
          end
          object rdoToFileSpecify: TRadioButton
            Left = 205
            Top = 402
            Width = 113
            Height = 17
            Caption = 'Output Directory'
            Enabled = False
            TabOrder = 14
            OnClick = rdoToFileSpecifyClick
          end
          object edtSuffix: TButtonedEdit
            Left = 72
            Top = 419
            Width = 109
            Height = 23
            Hint = '|Add Suffix to an output file.|2'
            Images = ImageList2
            RightButton.ImageIndex = 1
            TabOrder = 15
            OnChange = edtSuffixChange
            OnRightButtonClick = edtSuffixRightButtonClick
          end
          object lblOutputFolder: TButtonedEdit
            Left = 203
            Top = 419
            Width = 315
            Height = 23
            Hint = '|specify a destination of a file.|2'
            CustomHint = BalloonHint
            Images = ImageList2
            LeftButton.CustomHint = BalloonHint
            ParentShowHint = False
            ReadOnly = True
            RightButton.CustomHint = BalloonHint
            RightButton.ImageIndex = 0
            RightButton.Visible = True
            ShowHint = True
            TabOrder = 16
            OnRightButtonClick = lblOutputFolderRightButtonClick
          end
          object chkNormalize: TCheckBox
            Left = 430
            Top = 272
            Width = 93
            Height = 25
            Caption = 'Normalize'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = []
            ParentFont = False
            TabOrder = 17
          end
          object GroupBox9: TGroupBox
            Left = 211
            Top = 310
            Width = 321
            Height = 49
            Caption = 'Mode'
            TabOrder = 18
            object cmbParam: TComboBox
              Left = 8
              Top = 18
              Width = 299
              Height = 23
              Style = csDropDownList
              ItemIndex = 0
              TabOrder = 0
              Text = 'Normal'
              OnChange = cmbParamChange
              Items.Strings = (
                'Normal'
                'High Definition data(HD)'
                'High Definition data(DSD)'
                'Lossy compressed audio data(High bitrate MP3)'
                'Lossy compressed audio data(Low bitrate MP3)')
            end
          end
          object lblSamp: TEdit
            Left = 96
            Top = 162
            Width = 103
            Height = 23
            Color = clMoneyGreen
            ReadOnly = True
            TabOrder = 19
          end
          object lblWidth: TEdit
            Left = 272
            Top = 162
            Width = 68
            Height = 23
            Color = clMoneyGreen
            TabOrder = 20
          end
        end
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Extra'
      ImageIndex = 1
      object CategoryPanelGroup1: TCategoryPanelGroup
        Left = 0
        Top = 0
        Width = 568
        Height = 631
        VertScrollBar.Tracking = True
        Align = alClient
        ChevronColor = clNavy
        ChevronHotColor = clMaroon
        HeaderFont.Charset = DEFAULT_CHARSET
        HeaderFont.Color = clWindowText
        HeaderFont.Height = -11
        HeaderFont.Name = 'Tahoma'
        HeaderFont.Style = []
        TabOrder = 0
        object CategoryPanel9: TCategoryPanel
          Top = 595
          Height = 30
          Caption = 'DSF Option'
          Collapsed = True
          TabOrder = 0
          OnExpand = CategoryPanel9Expand
          ExpandedHeight = 505
          object Bevel23: TBevel
            Left = 7
            Top = 4
            Width = 510
            Height = 8
            Shape = bsBottomLine
          end
          object Label56: TLabel
            Left = 7
            Top = 14
            Width = 68
            Height = 15
            Caption = 'Filter Option'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object cmbDSF: TComboBox
            Left = 18
            Top = 41
            Width = 185
            Height = 23
            Style = csDropDownList
            ItemIndex = 0
            TabOrder = 0
            Text = 'None'
            Items.Strings = (
              'None'
              'Cutoff(24kHz)'
              'Smoothing')
          end
        end
        object CategoryPanel7: TCategoryPanel
          Top = 565
          Height = 30
          Caption = 'Hfa 2/3 Option'
          Collapsed = True
          TabOrder = 1
          OnExpand = CategoryPanel7Expand
          ExpandedHeight = 507
          object Label22: TLabel
            Left = 7
            Top = 13
            Width = 38
            Height = 15
            Caption = 'Preset'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Label13: TLabel
            Left = 33
            Top = 69
            Width = 23
            Height = 15
            Caption = 'Title'
          end
          object Bevel9: TBevel
            Left = 7
            Top = 99
            Width = 510
            Height = 8
            Shape = bsBottomLine
          end
          object Label23: TLabel
            Left = 7
            Top = 112
            Width = 25
            Height = 15
            Caption = 'Sig1'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Label7: TLabel
            Left = 35
            Top = 160
            Width = 57
            Height = 15
            Caption = 'avgLineNx'
          end
          object Label5: TLabel
            Left = 197
            Top = 160
            Width = 64
            Height = 15
            Caption = 'Sig1 Phase'
          end
          object Bevel11: TBevel
            Left = 7
            Top = 191
            Width = 510
            Height = 8
            Shape = bsBottomLine
          end
          object Label25: TLabel
            Left = 7
            Top = 200
            Width = 25
            Height = 15
            Caption = 'Sig2'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Label6: TLabel
            Left = 197
            Top = 227
            Width = 64
            Height = 15
            Caption = 'Sig2 Phase'
          end
          object Label27: TLabel
            Left = 8
            Top = 345
            Width = 67
            Height = 15
            Caption = 'Noise Blend'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Label9: TLabel
            Left = 23
            Top = 370
            Width = 64
            Height = 15
            Caption = 'Blend Level'
          end
          object Label10: TLabel
            Left = 183
            Top = 370
            Width = 11
            Height = 15
            Caption = '%'
          end
          object Bevel14: TBevel
            Left = 4
            Top = 399
            Width = 510
            Height = 8
            Shape = bsBottomLine
          end
          object Bevel8: TBevel
            Left = 7
            Top = 2
            Width = 510
            Height = 8
            Shape = bsBottomLine
          end
          object Bevel13: TBevel
            Left = 4
            Top = 335
            Width = 510
            Height = 8
            Shape = bsBottomLine
          end
          object Bevel2: TBevel
            Left = 8
            Top = 257
            Width = 510
            Height = 8
            Shape = bsBottomLine
          end
          object Label26: TLabel
            Left = 7
            Top = 268
            Width = 25
            Height = 15
            Caption = 'Sig3'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Label31: TLabel
            Left = 197
            Top = 296
            Width = 64
            Height = 15
            Caption = 'Sig3 Phase'
          end
          object Label71: TLabel
            Left = 261
            Top = 344
            Width = 43
            Height = 15
            Caption = 'Diff min'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object cmbHfaPreset: TComboBox
            Left = 23
            Top = 34
            Width = 309
            Height = 22
            Style = csOwnerDrawFixed
            TabOrder = 0
            OnChange = cmbHfaPresetChange
          end
          object edtPresetTitle: TEdit
            Left = 85
            Top = 66
            Width = 196
            Height = 23
            TabOrder = 1
          end
          object btnPreset: TButton
            Left = 291
            Top = 65
            Width = 41
            Height = 25
            Caption = 'Set'
            TabOrder = 2
            OnClick = btnPresetClick
          end
          object chkSig1Enb: TCheckBox
            Left = 23
            Top = 136
            Width = 69
            Height = 17
            Caption = 'Sig1 On'
            TabOrder = 3
          end
          object edtAvgLineNx: TEdit
            Left = 126
            Top = 156
            Width = 37
            Height = 23
            MaxLength = 6
            ReadOnly = True
            TabOrder = 4
            Text = '4'
          end
          object UpDown3: TUpDown
            Left = 163
            Top = 156
            Width = 15
            Height = 23
            Associate = edtAvgLineNx
            Min = 1
            Max = 25
            Position = 4
            TabOrder = 5
          end
          object edtSig1: TEdit
            Left = 291
            Top = 156
            Width = 37
            Height = 23
            MaxLength = 6
            ReadOnly = True
            TabOrder = 6
            Text = '0'
          end
          object UpDown1: TUpDown
            Left = 328
            Top = 156
            Width = 15
            Height = 23
            Associate = edtSig1
            Min = -44
            Max = 44
            TabOrder = 7
          end
          object chkSig2Enb: TCheckBox
            Left = 23
            Top = 224
            Width = 69
            Height = 17
            Caption = 'Sig2 On'
            TabOrder = 8
          end
          object edtSig2: TEdit
            Left = 291
            Top = 224
            Width = 37
            Height = 23
            MaxLength = -10
            ReadOnly = True
            TabOrder = 9
            Text = '0'
          end
          object UpDown2: TUpDown
            Left = 328
            Top = 224
            Width = 15
            Height = 23
            Associate = edtSig2
            Min = -44
            Max = 44
            TabOrder = 10
          end
          object edtNB: TEdit
            Left = 127
            Top = 366
            Width = 37
            Height = 23
            MaxLength = 6
            ReadOnly = True
            TabOrder = 11
            Text = '0'
          end
          object UpDown5: TUpDown
            Left = 164
            Top = 366
            Width = 15
            Height = 23
            Associate = edtNB
            Increment = 5
            TabOrder = 12
          end
          object chkHfaFast: TCheckBox
            Left = 23
            Top = 416
            Width = 202
            Height = 17
            Caption = 'Fast Mode(HFA2/HFA3)'
            TabOrder = 13
          end
          object edtHfaParam: TMemo
            Left = 15
            Top = 466
            Width = 458
            Height = 83
            TabOrder = 14
            Visible = False
          end
          object chkSig3Enb: TCheckBox
            Left = 23
            Top = 296
            Width = 69
            Height = 17
            Caption = 'Sig3 On'
            TabOrder = 15
          end
          object edtSig3: TEdit
            Left = 291
            Top = 293
            Width = 37
            Height = 23
            MaxLength = -10
            ReadOnly = True
            TabOrder = 16
            Text = '0'
          end
          object UpDown7: TUpDown
            Left = 328
            Top = 293
            Width = 15
            Height = 23
            Associate = edtSig3
            Min = -44
            Max = 44
            TabOrder = 17
          end
          object chkWideAna: TCheckBox
            Left = 231
            Top = 416
            Width = 192
            Height = 17
            Caption = 'Wide Analysis(HFA3)'
            TabOrder = 18
          end
          object edtDiffmin: TEdit
            Left = 291
            Top = 365
            Width = 37
            Height = 23
            MaxLength = 6
            ReadOnly = True
            TabOrder = 19
            Text = '1'
          end
          object UpDown8: TUpDown
            Left = 328
            Top = 365
            Width = 15
            Height = 23
            Associate = edtDiffmin
            Min = 1
            Max = 5
            Position = 1
            TabOrder = 20
          end
        end
        object CategoryPanel3: TCategoryPanel
          Top = 535
          Height = 30
          Caption = 'ABE Option'
          Collapsed = True
          TabOrder = 2
          OnExpand = CategoryPanel3Expand
          ExpandedHeight = 509
          object Label34: TLabel
            Left = 7
            Top = 17
            Width = 37
            Height = 15
            Caption = 'Option'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Label48: TLabel
            Left = 18
            Top = 92
            Width = 118
            Height = 15
            Caption = 'Cut Dither/Noise data'
          end
          object Bevel10: TBevel
            Left = 7
            Top = 6
            Width = 510
            Height = 8
            Shape = bsBottomLine
          end
          object chkCutLowData: TCheckBox
            Left = 18
            Top = 42
            Width = 251
            Height = 17
            Caption = 'Cut Low level data'
            TabOrder = 0
          end
          object cmbCutDither: TComboBox
            Left = 35
            Top = 118
            Width = 181
            Height = 23
            Style = csDropDownList
            ItemIndex = 0
            TabOrder = 1
            Text = 'Off'
            Items.Strings = (
              'Off'
              '1(Low)'
              '2'
              '3'
              '4'
              '5(High)')
          end
          object chkSMLowData: TCheckBox
            Left = 18
            Top = 66
            Width = 248
            Height = 17
            Caption = 'Smoothing Low level data'
            TabOrder = 2
          end
        end
        object CategoryPanel2: TCategoryPanel
          Top = 505
          Height = 30
          Caption = 'Option2'
          Collapsed = True
          Ctl3D = True
          Locked = True
          ParentCtl3D = False
          TabOrder = 3
          OnExpand = CategoryPanel2Expand
          ExpandedHeight = 509
          object Bevel3: TBevel
            Left = 9
            Top = 127
            Width = 510
            Height = 6
            Shape = bsBottomLine
          end
          object Label17: TLabel
            Left = 9
            Top = 136
            Width = 92
            Height = 15
            Caption = 'Noise Reduction'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Label4: TLabel
            Left = 24
            Top = 157
            Width = 60
            Height = 15
            Caption = 'Cutoff Freq'
          end
          object Label11: TLabel
            Left = 194
            Top = 157
            Width = 46
            Height = 15
            Caption = 'NR level'
          end
          object Bevel7: TBevel
            Left = 9
            Top = 193
            Width = 510
            Height = 6
            Shape = bsBottomLine
          end
          object Label18: TLabel
            Left = 9
            Top = 201
            Width = 84
            Height = 15
            Caption = 'Low pass filter'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Label1: TLabel
            Left = 24
            Top = 226
            Width = 60
            Height = 15
            Caption = 'Cutoff Freq'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = []
            ParentFont = False
          end
          object Label32: TLabel
            Left = 8
            Top = 4
            Width = 43
            Height = 15
            Caption = 'Feature'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Bevel16: TBevel
            Left = 9
            Top = 18
            Width = 510
            Height = 6
            Shape = bsBottomLine
          end
          object chkDeEmphasis: TCheckBox
            Left = 15
            Top = 76
            Width = 97
            Height = 17
            Caption = 'De-Emphasis'
            TabOrder = 0
          end
          object rdoDEType1: TRadioButton
            Left = 39
            Top = 99
            Width = 113
            Height = 17
            Hint = '|De-Emphasis for CD|2'
            CustomHint = BalloonHint
            Caption = '50/15us(CD-DA)'
            Checked = True
            ParentShowHint = False
            ShowHint = True
            TabOrder = 1
            TabStop = True
          end
          object rdoDEType2: TRadioButton
            Left = 250
            Top = 99
            Width = 113
            Height = 17
            Hint = '|De-Emphasis for other stream|2'
            CustomHint = BalloonHint
            Caption = 'CCITT J.17'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 2
          end
          object edtNR: TEdit
            Left = 118
            Top = 154
            Width = 61
            Height = 23
            Ctl3D = True
            MaxLength = 6
            NumbersOnly = True
            ParentCtl3D = False
            TabOrder = 3
          end
          object edtNRLV: TEdit
            Left = 284
            Top = 154
            Width = 37
            Height = 23
            Ctl3D = True
            NumbersOnly = True
            ParentCtl3D = False
            ReadOnly = True
            TabOrder = 4
            Text = '1'
          end
          object UpDown6: TUpDown
            Left = 321
            Top = 154
            Width = 15
            Height = 23
            Associate = edtNRLV
            Min = 1
            Max = 5
            Position = 1
            TabOrder = 5
          end
          object edtLPF: TEdit
            Left = 118
            Top = 220
            Width = 61
            Height = 23
            MaxLength = 6
            NumbersOnly = True
            TabOrder = 6
          end
          object chkAdjFreq: TCheckBox
            Left = 15
            Top = 30
            Width = 140
            Height = 17
            Caption = 'SP Frequency Adjust'
            TabOrder = 7
          end
          object chkEQ: TCheckBox
            Left = 15
            Top = 53
            Width = 97
            Height = 17
            Caption = 'Equalizer'
            TabOrder = 8
          end
        end
        object CategoryPanel1: TCategoryPanel
          Top = 0
          Height = 505
          Caption = 'Option1'
          Ctl3D = False
          Locked = True
          ParentCtl3D = False
          TabOrder = 4
          OnExpand = CategoryPanel1Expand
          object Label15: TLabel
            Left = 7
            Top = 14
            Width = 44
            Height = 15
            Caption = 'Convert'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Bevel6: TBevel
            Left = 7
            Top = 4
            Width = 510
            Height = 8
            Shape = bsBottomLine
          end
          object Bevel5: TBevel
            Left = 7
            Top = 189
            Width = 510
            Height = 8
            Shape = bsBottomLine
          end
          object Label12: TLabel
            Left = 7
            Top = 200
            Width = 81
            Height = 15
            Caption = 'Output Format'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Bevel15: TBevel
            Left = 7
            Top = 128
            Width = 510
            Height = 8
            Shape = bsBottomLine
          end
          object Label57: TLabel
            Left = 7
            Top = 138
            Width = 72
            Height = 15
            Caption = 'Output Mode'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Label3: TLabel
            Left = 256
            Top = 164
            Width = 61
            Height = 15
            Caption = 'Dither level'
          end
          object Bevel24: TBevel
            Left = 7
            Top = 253
            Width = 510
            Height = 8
            Shape = bsBottomLine
          end
          object Label58: TLabel
            Left = 7
            Top = 265
            Width = 47
            Height = 15
            Caption = 'Encoder'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Label20: TLabel
            Left = 52
            Top = 335
            Width = 69
            Height = 15
            Caption = 'FLAC Option'
          end
          object Label21: TLabel
            Left = 52
            Top = 369
            Width = 92
            Height = 15
            Caption = 'Wav Pack Option'
          end
          object chkOverSampling: TCheckBox
            Left = 256
            Top = 58
            Width = 129
            Height = 17
            Caption = 'OverSampling(x2)'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = []
            ParentFont = False
            TabOrder = 0
          end
          object chkLfa: TCheckBox
            Left = 23
            Top = 105
            Width = 167
            Height = 17
            Caption = 'Low Adjust(100Hz)'
            TabOrder = 1
          end
          object chkHdc: TCheckBox
            Left = 23
            Top = 82
            Width = 192
            Height = 17
            Caption = 'Cut High Pass Dither'
            TabOrder = 2
          end
          object chkHfcAuto: TCheckBox
            Left = 23
            Top = 58
            Width = 97
            Height = 17
            Caption = 'HFC Auto'
            TabOrder = 3
          end
          object chkOs32x: TCheckBox
            Left = 256
            Top = 82
            Width = 195
            Height = 17
            Caption = 'OverSampling(768000)'
            TabOrder = 4
          end
          object chkAdjBE: TCheckBox
            Left = 23
            Top = 35
            Width = 165
            Height = 17
            Caption = 'Adjust Bit Extension'
            TabOrder = 5
          end
          object cmbBOM: TComboBox
            Left = 23
            Top = 161
            Width = 175
            Height = 22
            Style = csOwnerDrawFixed
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = []
            ItemIndex = 0
            ParentFont = False
            TabOrder = 6
            Text = 'Cutoff'
            Items.Strings = (
              'Cutoff'
              'Dithering'
              'Noise shaping'
              'Error Diffusion Method')
          end
          object edtDitherLevel: TEdit
            Left = 353
            Top = 161
            Width = 43
            Height = 23
            Ctl3D = True
            NumbersOnly = True
            ParentCtl3D = False
            ReadOnly = True
            TabOrder = 7
            Text = '0'
          end
          object UpDown4: TUpDown
            Left = 396
            Top = 161
            Width = 15
            Height = 23
            Associate = edtDitherLevel
            Max = 16
            TabOrder = 8
          end
          object cmbOutFormat: TComboBox
            Left = 23
            Top = 224
            Width = 175
            Height = 23
            Style = csDropDownList
            ItemIndex = 0
            TabOrder = 9
            Text = 'WAV(Microsoft)'
            Items.Strings = (
              'WAV(Microsoft)'
              'BWF(Broadcast Wave Format)'
              'RF64'
              'W64'
              'AIFF')
          end
          object cmbEncoder: TComboBox
            Left = 23
            Top = 291
            Width = 175
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 10
            Text = 'None'
            Items.Strings = (
              'None'
              'FLAC'
              'WavPack')
          end
          object edtFLACOption: TEdit
            Left = 179
            Top = 329
            Width = 265
            Height = 23
            Ctl3D = True
            ParentCtl3D = False
            TabOrder = 11
          end
          object edtWavPackOption: TEdit
            Left = 179
            Top = 363
            Width = 265
            Height = 23
            Ctl3D = True
            ParentCtl3D = False
            TabOrder = 12
          end
          object chkPostABE: TCheckBox
            Left = 256
            Top = 35
            Width = 184
            Height = 17
            Caption = 'Post ABE'
            TabOrder = 13
          end
        end
      end
    end
    object TabSheet1: TTabSheet
      Caption = 'EQ'
      ImageIndex = 2
      object CategoryPanelGroup3: TCategoryPanelGroup
        Left = 0
        Top = 0
        Width = 568
        Height = 631
        VertScrollBar.Tracking = True
        Align = alClient
        HeaderFont.Charset = DEFAULT_CHARSET
        HeaderFont.Color = clWindowText
        HeaderFont.Height = -11
        HeaderFont.Name = 'Tahoma'
        HeaderFont.Style = []
        TabOrder = 0
        object CategoryPanel4: TCategoryPanel
          Top = 0
          Height = 627
          Caption = 'EQ'
          TabOrder = 0
          object Label35: TLabel
            Left = 467
            Top = 375
            Width = 50
            Height = 15
            Caption = '18k - 20k'
          end
          object Label36: TLabel
            Left = 411
            Top = 375
            Width = 50
            Height = 15
            Caption = '16k - 18k'
          end
          object Label37: TLabel
            Left = 357
            Top = 375
            Width = 50
            Height = 15
            Caption = '14k - 16k'
          end
          object Label38: TLabel
            Left = 299
            Top = 375
            Width = 50
            Height = 15
            Caption = '12k - 14k'
          end
          object Label39: TLabel
            Left = 239
            Top = 375
            Width = 50
            Height = 15
            Caption = '10k - 12k'
          end
          object Label40: TLabel
            Left = 183
            Top = 375
            Width = 43
            Height = 15
            Caption = '8k - 10k'
          end
          object Label41: TLabel
            Left = 128
            Top = 375
            Width = 36
            Height = 15
            Caption = '6k - 8k'
          end
          object Label42: TLabel
            Left = 15
            Top = 375
            Width = 36
            Height = 15
            Caption = '2k - 4k'
          end
          object Label43: TLabel
            Left = 69
            Top = 375
            Width = 36
            Height = 15
            Caption = '4k - 6k'
          end
          object Label44: TLabel
            Left = 471
            Top = 186
            Width = 36
            Height = 15
            Caption = '1k - 2k'
          end
          object Label45: TLabel
            Left = 357
            Top = 186
            Width = 52
            Height = 15
            Caption = '500 - 750'
          end
          object Label46: TLabel
            Left = 237
            Top = 186
            Width = 52
            Height = 15
            Caption = '100 - 250'
          end
          object Bevel20: TBevel
            Left = 19
            Top = 420
            Width = 469
            Height = 8
            Shape = bsBottomLine
          end
          object Label33: TLabel
            Left = 533
            Top = 291
            Width = 7
            Height = 15
            Caption = '0'
          end
          object Label49: TLabel
            Left = 5
            Top = 6
            Width = 52
            Height = 15
            Caption = 'Equalizer'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Bevel19: TBevel
            Left = 5
            Top = 16
            Width = 510
            Height = 8
            Shape = bsBottomLine
          end
          object Bevel30: TBevel
            Left = 9
            Top = 209
            Width = 510
            Height = 8
            Shape = bsBottomLine
          end
          object Label47: TLabel
            Left = 15
            Top = 186
            Width = 38
            Height = 15
            Caption = '20 - 40'
          end
          object Label65: TLabel
            Left = 71
            Top = 186
            Width = 38
            Height = 15
            Caption = '40 - 60'
          end
          object Label66: TLabel
            Left = 130
            Top = 186
            Width = 38
            Height = 15
            Caption = '60 - 80'
          end
          object Label67: TLabel
            Left = 186
            Top = 186
            Width = 45
            Height = 15
            Caption = '80 - 100'
          end
          object Label68: TLabel
            Left = 299
            Top = 186
            Width = 52
            Height = 15
            Caption = '250 - 500'
          end
          object Label69: TLabel
            Left = 415
            Top = 186
            Width = 44
            Height = 15
            Caption = '750 - 1k'
          end
          object Label70: TLabel
            Left = 533
            Top = 100
            Width = 7
            Height = 15
            Caption = '0'
          end
          object eq100: TTrackBar
            Left = 256
            Top = 33
            Width = 21
            Height = 151
            Orientation = trVertical
            PageSize = 1
            Position = 5
            ShowSelRange = False
            TabOrder = 0
            ThumbLength = 15
            TickMarks = tmBoth
            TickStyle = tsNone
          end
          object eq250: TTrackBar
            Left = 315
            Top = 33
            Width = 21
            Height = 151
            Orientation = trVertical
            PageSize = 1
            Position = 5
            ShowSelRange = False
            TabOrder = 1
            ThumbLength = 15
            TickMarks = tmBoth
            TickStyle = tsNone
          end
          object eq500: TTrackBar
            Left = 373
            Top = 33
            Width = 21
            Height = 151
            Orientation = trVertical
            PageSize = 1
            Position = 5
            ShowSelRange = False
            TabOrder = 2
            ThumbLength = 15
            TickMarks = tmBoth
            TickStyle = tsNone
          end
          object eq2k: TTrackBar
            Left = 25
            Top = 223
            Width = 21
            Height = 151
            Orientation = trVertical
            PageSize = 1
            Position = 5
            ShowSelRange = False
            TabOrder = 3
            ThumbLength = 15
            TickMarks = tmBoth
            TickStyle = tsNone
          end
          object eq4k: TTrackBar
            Left = 80
            Top = 223
            Width = 21
            Height = 151
            Orientation = trVertical
            PageSize = 1
            Position = 5
            ShowSelRange = False
            TabOrder = 4
            ThumbLength = 15
            TickMarks = tmBoth
            TickStyle = tsNone
          end
          object eq6k: TTrackBar
            Left = 139
            Top = 223
            Width = 21
            Height = 151
            Orientation = trVertical
            PageSize = 1
            Position = 5
            ShowSelRange = False
            TabOrder = 5
            ThumbLength = 15
            TickMarks = tmBoth
            TickStyle = tsNone
          end
          object eq8k: TTrackBar
            Left = 197
            Top = 223
            Width = 21
            Height = 151
            Orientation = trVertical
            PageSize = 1
            Position = 5
            ShowSelRange = False
            TabOrder = 6
            ThumbLength = 15
            TickMarks = tmBoth
            TickStyle = tsNone
          end
          object eq10k: TTrackBar
            Left = 256
            Top = 223
            Width = 21
            Height = 151
            Orientation = trVertical
            PageSize = 1
            Position = 5
            ShowSelRange = False
            TabOrder = 7
            ThumbLength = 15
            TickMarks = tmBoth
            TickStyle = tsNone
          end
          object eq12k: TTrackBar
            Left = 315
            Top = 223
            Width = 21
            Height = 151
            Orientation = trVertical
            PageSize = 1
            Position = 5
            ShowSelRange = False
            TabOrder = 8
            ThumbLength = 15
            TickMarks = tmBoth
            TickStyle = tsNone
          end
          object eq14k: TTrackBar
            Left = 373
            Top = 223
            Width = 21
            Height = 151
            Orientation = trVertical
            PageSize = 1
            Position = 5
            ShowSelRange = False
            TabOrder = 9
            ThumbLength = 15
            TickMarks = tmBoth
            TickStyle = tsNone
          end
          object eq16k: TTrackBar
            Left = 429
            Top = 223
            Width = 21
            Height = 151
            Orientation = trVertical
            PageSize = 1
            Position = 5
            ShowSelRange = False
            TabOrder = 10
            ThumbLength = 15
            TickMarks = tmBoth
            TickStyle = tsNone
          end
          object eq18k: TTrackBar
            Left = 482
            Top = 224
            Width = 21
            Height = 151
            Orientation = trVertical
            PageSize = 1
            Position = 5
            ShowSelRange = False
            TabOrder = 11
            ThumbLength = 15
            TickMarks = tmBoth
            TickStyle = tsNone
          end
          object btnEqSave: TBitBtn
            Left = 26
            Top = 447
            Width = 75
            Height = 25
            Caption = 'Save'
            Default = True
            DoubleBuffered = True
            Glyph.Data = {
              DE010000424DDE01000000000000760000002800000024000000120000000100
              0400000000006801000000000000000000001000000000000000000000000000
              80000080000000808000800000008000800080800000C0C0C000808080000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
              3333333333333333333333330000333333333333333333333333F33333333333
              00003333344333333333333333388F3333333333000033334224333333333333
              338338F3333333330000333422224333333333333833338F3333333300003342
              222224333333333383333338F3333333000034222A22224333333338F338F333
              8F33333300003222A3A2224333333338F3838F338F33333300003A2A333A2224
              33333338F83338F338F33333000033A33333A222433333338333338F338F3333
              0000333333333A222433333333333338F338F33300003333333333A222433333
              333333338F338F33000033333333333A222433333333333338F338F300003333
              33333333A222433333333333338F338F00003333333333333A22433333333333
              3338F38F000033333333333333A223333333333333338F830000333333333333
              333A333333333333333338330000333333333333333333333333333333333333
              0000}
            ModalResult = 1
            NumGlyphs = 2
            ParentDoubleBuffered = False
            TabOrder = 12
            OnClick = btnEqSaveClick
          end
          object BtnReset: TBitBtn
            Left = 116
            Top = 447
            Width = 75
            Height = 25
            Cancel = True
            Caption = 'Reset'
            DoubleBuffered = True
            Glyph.Data = {
              DE010000424DDE01000000000000760000002800000024000000120000000100
              0400000000006801000000000000000000001000000000000000000000000000
              80000080000000808000800000008000800080800000C0C0C000808080000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
              333333333333333333333333000033338833333333333333333F333333333333
              0000333911833333983333333388F333333F3333000033391118333911833333
              38F38F333F88F33300003339111183911118333338F338F3F8338F3300003333
              911118111118333338F3338F833338F3000033333911111111833333338F3338
              3333F8330000333333911111183333333338F333333F83330000333333311111
              8333333333338F3333383333000033333339111183333333333338F333833333
              00003333339111118333333333333833338F3333000033333911181118333333
              33338333338F333300003333911183911183333333383338F338F33300003333
              9118333911183333338F33838F338F33000033333913333391113333338FF833
              38F338F300003333333333333919333333388333338FFF830000333333333333
              3333333333333333333888330000333333333333333333333333333333333333
              0000}
            ModalResult = 2
            NumGlyphs = 2
            ParentDoubleBuffered = False
            TabOrder = 13
            OnClick = BtnResetClick
          end
          object eq20: TTrackBar
            Left = 26
            Top = 33
            Width = 21
            Height = 151
            Orientation = trVertical
            PageSize = 1
            Position = 5
            ShowSelRange = False
            TabOrder = 14
            ThumbLength = 15
            TickMarks = tmBoth
            TickStyle = tsNone
          end
          object eq40: TTrackBar
            Left = 82
            Top = 33
            Width = 21
            Height = 151
            Orientation = trVertical
            PageSize = 1
            Position = 5
            ShowSelRange = False
            TabOrder = 15
            ThumbLength = 15
            TickMarks = tmBoth
            TickStyle = tsNone
          end
          object eq60: TTrackBar
            Left = 139
            Top = 33
            Width = 21
            Height = 151
            Orientation = trVertical
            PageSize = 1
            Position = 5
            ShowSelRange = False
            TabOrder = 16
            ThumbLength = 15
            TickMarks = tmBoth
            TickStyle = tsNone
          end
          object eq80: TTrackBar
            Left = 197
            Top = 33
            Width = 21
            Height = 151
            Orientation = trVertical
            PageSize = 1
            Position = 5
            ShowSelRange = False
            TabOrder = 17
            ThumbLength = 15
            TickMarks = tmBoth
            TickStyle = tsNone
          end
          object eq750: TTrackBar
            Left = 429
            Top = 33
            Width = 21
            Height = 151
            Orientation = trVertical
            PageSize = 1
            Position = 5
            ShowSelRange = False
            TabOrder = 18
            ThumbLength = 15
            TickMarks = tmBoth
            TickStyle = tsNone
          end
          object eq1k: TTrackBar
            Left = 482
            Top = 33
            Width = 21
            Height = 151
            Orientation = trVertical
            PageSize = 1
            Position = 5
            ShowSelRange = False
            TabOrder = 19
            ThumbLength = 15
            TickMarks = tmBoth
            TickStyle = tsNone
          end
        end
      end
    end
    object TabSheet6: TTabSheet
      Caption = 'Multi Channel'
      ImageIndex = 5
      object CategoryPanelGroup6: TCategoryPanelGroup
        Left = 0
        Top = 0
        Width = 568
        Height = 631
        VertScrollBar.Tracking = True
        Align = alClient
        HeaderFont.Charset = DEFAULT_CHARSET
        HeaderFont.Color = clWindowText
        HeaderFont.Height = -11
        HeaderFont.Name = 'Tahoma'
        HeaderFont.Style = []
        TabOrder = 0
        object CategoryPanel10: TCategoryPanel
          Top = 0
          Height = 628
          Caption = 'Multi Channel Gen'
          TabOrder = 0
          object Bevel26: TBevel
            Left = 5
            Top = 16
            Width = 510
            Height = 8
            Shape = bsBottomLine
          end
          object Label60: TLabel
            Left = 5
            Top = 6
            Width = 38
            Height = 15
            Caption = 'Center'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Bevel27: TBevel
            Left = 5
            Top = 80
            Width = 510
            Height = 8
            Shape = bsBottomLine
          end
          object Label61: TLabel
            Left = 5
            Top = 68
            Width = 53
            Height = 15
            Caption = 'Surround'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Label62: TLabel
            Left = 5
            Top = 136
            Width = 20
            Height = 15
            Caption = 'LFE'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Bevel28: TBevel
            Left = 5
            Top = 150
            Width = 510
            Height = 8
            Shape = bsBottomLine
          end
          object Bevel29: TBevel
            Left = 7
            Top = 223
            Width = 510
            Height = 8
            Shape = bsBottomLine
          end
          object Label63: TLabel
            Left = 7
            Top = 208
            Width = 38
            Height = 15
            Caption = 'Output'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Label64: TLabel
            Left = 20
            Top = 323
            Width = 62
            Height = 15
            Caption = 'Effect Level'
          end
          object chkGenCenter: TCheckBox
            Left = 20
            Top = 34
            Width = 289
            Height = 17
            Caption = 'Generate Center Channel'
            TabOrder = 0
          end
          object chkGenSurround: TCheckBox
            Left = 20
            Top = 100
            Width = 289
            Height = 17
            Caption = 'Generate Surround Left/Right'
            TabOrder = 1
          end
          object chkGenLFE: TCheckBox
            Left = 20
            Top = 172
            Width = 289
            Height = 17
            Caption = 'Generate LFE'
            TabOrder = 2
          end
          object chkOutMono: TCheckBox
            Left = 20
            Top = 240
            Width = 273
            Height = 17
            Caption = 'Output Monaural Files'
            TabOrder = 3
          end
          object chkechoeffect: TCheckBox
            Left = 20
            Top = 297
            Width = 289
            Height = 17
            Caption = 'Echo Effect'
            TabOrder = 4
          end
          object chkDownMix: TCheckBox
            Left = 20
            Top = 269
            Width = 241
            Height = 17
            Caption = 'Down Mix'
            TabOrder = 5
          end
          object cmbEffect: TComboBox
            Left = 170
            Top = 320
            Width = 145
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 6
            Text = '1'
            Items.Strings = (
              '1'
              '2'
              '3'
              '4'
              '5')
          end
        end
      end
    end
    object TabSheet4: TTabSheet
      Caption = 'Run'
      ImageIndex = 3
      object CategoryPanelGroup4: TCategoryPanelGroup
        Left = 0
        Top = 0
        Width = 568
        Height = 631
        VertScrollBar.Tracking = True
        Align = alClient
        HeaderFont.Charset = DEFAULT_CHARSET
        HeaderFont.Color = clWindowText
        HeaderFont.Height = -11
        HeaderFont.Name = 'Tahoma'
        HeaderFont.Style = []
        TabOrder = 0
        object CategoryPanel6: TCategoryPanel
          Top = 0
          Height = 627
          Caption = 'User Command'
          TabOrder = 0
          object Bevel21: TBevel
            Left = 3
            Top = 10
            Width = 6
            Height = 487
            Shape = bsRightLine
          end
          object Label50: TLabel
            Left = 15
            Top = 10
            Width = 50
            Height = 15
            Caption = 'Program'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Label51: TLabel
            Left = 15
            Top = 66
            Width = 61
            Height = 15
            Caption = 'Parameter'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Label52: TLabel
            Left = 15
            Top = 128
            Width = 50
            Height = 15
            Caption = 'Input File'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Label53: TLabel
            Left = 15
            Top = 192
            Width = 50
            Height = 15
            Caption = 'Save File'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Label54: TLabel
            Left = 15
            Top = 284
            Width = 85
            Height = 15
            Caption = 'Command Line'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Bevel22: TBevel
            Left = 15
            Top = 274
            Width = 469
            Height = 8
            Shape = bsBottomLine
          end
          object Label55: TLabel
            Left = 15
            Top = 340
            Width = 36
            Height = 15
            Caption = 'Result'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object edtUsrCmd: TButtonedEdit
            Left = 30
            Top = 31
            Width = 393
            Height = 23
            Images = ImageList2
            LeftButton.ImageIndex = 1
            LeftButton.Visible = True
            ReadOnly = True
            RightButton.ImageIndex = 0
            RightButton.Visible = True
            TabOrder = 0
            OnLeftButtonClick = edtUsrCmdLeftButtonClick
            OnRightButtonClick = edtUsrCmdRightButtonClick
          end
          object edtUsrParam: TButtonedEdit
            Left = 30
            Top = 92
            Width = 325
            Height = 23
            Images = ImageList2
            LeftButton.ImageIndex = 1
            LeftButton.Visible = True
            TabOrder = 1
            OnLeftButtonClick = edtUsrParamLeftButtonClick
          end
          object edtUsrInput: TButtonedEdit
            Left = 30
            Top = 157
            Width = 325
            Height = 23
            Images = ImageList2
            LeftButton.ImageIndex = 1
            LeftButton.Visible = True
            ReadOnly = True
            RightButton.ImageIndex = 0
            RightButton.Visible = True
            TabOrder = 2
            OnLeftButtonClick = edtUsrInputLeftButtonClick
            OnRightButtonClick = edtUsrInputRightButtonClick
          end
          object btnUsrInputSet: TBitBtn
            Left = 366
            Top = 156
            Width = 57
            Height = 25
            Caption = 'Set'
            Default = True
            DoubleBuffered = True
            ModalResult = 1
            NumGlyphs = 2
            ParentDoubleBuffered = False
            TabOrder = 3
            OnClick = btnUsrInputSetClick
          end
          object edtUsrOutput: TButtonedEdit
            Left = 30
            Top = 219
            Width = 325
            Height = 23
            Images = ImageList2
            LeftButton.ImageIndex = 1
            LeftButton.Visible = True
            ReadOnly = True
            RightButton.ImageIndex = 0
            RightButton.Visible = True
            TabOrder = 4
            OnLeftButtonClick = edtUsrOutputLeftButtonClick
            OnRightButtonClick = edtUsrOutputRightButtonClick
          end
          object btnUsrOutputSet: TBitBtn
            Left = 366
            Top = 218
            Width = 57
            Height = 25
            Caption = 'Set'
            Default = True
            DoubleBuffered = True
            ModalResult = 1
            NumGlyphs = 2
            ParentDoubleBuffered = False
            TabOrder = 5
            OnClick = btnUsrOutputSetClick
          end
          object edtUsrCmdLine: TButtonedEdit
            Left = 30
            Top = 311
            Width = 393
            Height = 23
            AutoSelect = False
            BevelEdges = []
            BevelOuter = bvNone
            Ctl3D = True
            Images = ImageList2
            LeftButton.ImageIndex = 1
            LeftButton.Visible = True
            ParentCtl3D = False
            RightButton.Enabled = False
            RightButton.ImageIndex = 0
            TabOrder = 6
            OnLeftButtonClick = edtUsrCmdLineLeftButtonClick
          end
          object edtUsrStatus: TMemo
            Left = 30
            Top = 360
            Width = 393
            Height = 91
            ReadOnly = True
            ScrollBars = ssBoth
            TabOrder = 7
          end
          object btnUsrExec: TBitBtn
            Left = 30
            Top = 466
            Width = 75
            Height = 25
            Caption = 'Start'
            Default = True
            DoubleBuffered = True
            Glyph.Data = {
              DE010000424DDE01000000000000760000002800000024000000120000000100
              0400000000006801000000000000000000001000000000000000000000000000
              80000080000000808000800000008000800080800000C0C0C000808080000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
              3333333333333333333333330000333333333333333333333333F33333333333
              00003333344333333333333333388F3333333333000033334224333333333333
              338338F3333333330000333422224333333333333833338F3333333300003342
              222224333333333383333338F3333333000034222A22224333333338F338F333
              8F33333300003222A3A2224333333338F3838F338F33333300003A2A333A2224
              33333338F83338F338F33333000033A33333A222433333338333338F338F3333
              0000333333333A222433333333333338F338F33300003333333333A222433333
              333333338F338F33000033333333333A222433333333333338F338F300003333
              33333333A222433333333333338F338F00003333333333333A22433333333333
              3338F38F000033333333333333A223333333333333338F830000333333333333
              333A333333333333333338330000333333333333333333333333333333333333
              0000}
            ModalResult = 1
            NumGlyphs = 2
            ParentDoubleBuffered = False
            TabOrder = 8
            OnClick = btnUsrExecClick
          end
          object btnUsrParamSet: TBitBtn
            Left = 366
            Top = 91
            Width = 57
            Height = 25
            Caption = 'Set'
            Default = True
            DoubleBuffered = True
            ModalResult = 1
            NumGlyphs = 2
            ParentDoubleBuffered = False
            TabOrder = 9
            OnClick = btnUsrParamSetClick
          end
        end
      end
    end
    object TabSheet5: TTabSheet
      Caption = 'Option'
      ImageIndex = 4
      object CategoryPanelGroup5: TCategoryPanelGroup
        Left = 0
        Top = 0
        Width = 568
        Height = 631
        VertScrollBar.Tracking = True
        Align = alClient
        HeaderFont.Charset = DEFAULT_CHARSET
        HeaderFont.Color = clWindowText
        HeaderFont.Height = -11
        HeaderFont.Name = 'Tahoma'
        HeaderFont.Style = []
        TabOrder = 0
        object CategoryPanel5: TCategoryPanel
          Top = 0
          Height = 627
          Caption = 'Option'
          TabOrder = 0
          object Label24: TLabel
            Left = 7
            Top = 5
            Width = 94
            Height = 15
            Caption = 'Complate Option'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Bevel4: TBevel
            Left = 7
            Top = 21
            Width = 469
            Height = 6
            Shape = bsBottomLine
          end
          object Label14: TLabel
            Left = 7
            Top = 95
            Width = 40
            Height = 15
            Caption = 'Thread'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Bevel18: TBevel
            Left = 7
            Top = 111
            Width = 469
            Height = 6
            Shape = bsBottomLine
          end
          object Label59: TLabel
            Left = 7
            Top = 171
            Width = 150
            Height = 15
            Caption = 'File I/O Memory Buffer(MB)'
            Color = cl3DLight
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'FixedSys'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
          end
          object Bevel25: TBevel
            Left = 7
            Top = 161
            Width = 469
            Height = 6
            Shape = bsBottomLine
          end
          object chkDeleteFiles: TCheckBox
            Left = 15
            Top = 37
            Width = 138
            Height = 17
            Caption = 'Delete source File'
            TabOrder = 0
          end
          object chkShutdown: TCheckBox
            Left = 15
            Top = 63
            Width = 97
            Height = 17
            Caption = 'Power off'
            TabOrder = 1
          end
          object cmbThread: TComboBox
            Left = 15
            Top = 126
            Width = 158
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 2
            Text = '1'
            Items.Strings = (
              '1'
              '2'
              '3'
              '4'
              '5'
              '6'
              '7'
              '8'
              '9'
              '10'
              '11'
              '12'
              '13'
              '14'
              '15'
              '16'
              '17'
              '18'
              '19'
              '20'
              '21'
              '22'
              '23'
              '24')
          end
          object cmbIO: TComboBox
            Left = 15
            Top = 202
            Width = 158
            Height = 23
            Style = csDropDownList
            ItemIndex = 0
            TabOrder = 3
            Text = '30'
            Items.Strings = (
              '30'
              '50'
              '100'
              '200'
              '400'
              '600'
              '800'
              '1000'
              '1500'
              '2000'
              '4000'
              '6000'
              '8000'
              '10000'
              '12000'
              '14000')
          end
          object btnIOAdjust: TButton
            Left = 194
            Top = 202
            Width = 85
            Height = 23
            Caption = 'Adjust'
            TabOrder = 4
            OnClick = btnIOAdjustClick
          end
        end
      end
    end
  end
  object OpenDialog: TOpenDialog
    Filter = 'WAV|*.WAV'
    Options = [ofReadOnly, ofAllowMultiSelect, ofPathMustExist, ofFileMustExist, ofNoNetworkButton]
    Left = 254
    Top = 548
  end
  object ImageList: TImageList
    Left = 298
    Top = 550
    Bitmap = {
      494C010104005C055C0510001000FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
      0000000000003600000028000000400000002000000001002000000000000020
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000007F7F7F007F7F7F007F7F
      7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F
      7F007F7F7F007F7F7F0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000007F7F7F007F7F7F007F7F
      7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F
      7F007F7F7F007F7F7F0000000000000000000000FF007F7F7F007F7F7F007F7F
      7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F
      7F007F7F7F007F7F7F00FFFFFF00FFFFFF00000000007F7F7F00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000007F7F7F0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000007F7F7F00FFFF0000FFFF
      0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF
      0000FFFF00007F7F7F0000000000000000000000FF000000FF000000FF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF007F7F7F00FFFFFF00FFFFFF00000000007F7F7F00000000000000
      0000BFBFBF00BFBFBF00BFBFBF00000000000000000000000000000000000000
      0000000000007F7F7F0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000080800000000000000000
      000000000000000000000000000000000000000000007F7F7F00FFFF0000FFFF
      0000BFBFBF00BFBFBF00BFBFBF00FFFF0000FFFF0000FFFF0000FFFF0000FFFF
      0000FFFF00007F7F7F000000000000000000FFFFFF000000FF000000FF000000
      FF00BFBFBF00BFBFBF00BFBFBF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF007F7F7F00FFFFFF00FFFFFF00000000007F7F7F00000000000000
      00008080000080800000808000007F7F7F000000000000000000000000000000
      0000000000007F7F7F0000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFF0000808000000000
      000000000000000000000000000000000000000000007F7F7F00FFFF0000FFFF
      00008080000080800000808000007F7F7F00FFFF0000FFFF0000FFFF0000FFFF
      0000FFFF00007F7F7F000000000000000000FFFFFF007F7F7F000000FF000000
      FF000000FF0080800000808000007F7F7F00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF007F7F7F00FFFFFF00FFFFFF00000000007F7F7F00000000000000
      0000FFFF0000808000008080000080800000BFBFBF0000000000000000000000
      0000000000007F7F7F0000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFF0000FFFF00008080
      000000000000000000000000000000000000000000007F7F7F00FFFF0000FFFF
      0000FFFF0000808000008080000080800000BFBFBF00FFFF0000FFFF0000FFFF
      0000FFFF00007F7F7F000000000000000000FFFFFF007F7F7F00FFFFFF000000
      FF000000FF000000FF008080000080800000BFBFBF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF007F7F7F00FFFFFF00FFFFFF00000000007F7F7F00000000000000
      00007F7F7F0080800000FFFF000080800000BFBFBF00BFBFBF00000000000000
      0000000000007F7F7F0000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFF0000FFFF0000FFFF
      000080800000000000000000000000000000000000007F7F7F00FFFF0000FFFF
      00007F7F7F0080800000FFFF000080800000BFBFBF00BFBFBF00FFFF0000FFFF
      0000FFFF00007F7F7F000000000000000000FFFFFF007F7F7F00FFFFFF00FFFF
      FF000000FF000000FF000000FF0080800000BFBFBF00BFBFBF00FFFFFF00FFFF
      FF00FFFFFF007F7F7F00FFFFFF00FFFFFF00000000007F7F7F00000000000000
      00000000000000000000BFBFBF0080800000BFBFBF0080800000000000000000
      0000000000007F7F7F0000000000000000000000000000000000808000008080
      00008080000080800000808000008080000080800000FFFF0000FFFF0000FFFF
      0000FFFF0000808000000000000000000000000000007F7F7F00FFFF0000FFFF
      0000FFFF0000FFFF0000BFBFBF0080800000BFBFBF0080800000FFFF0000FFFF
      0000FFFF00007F7F7F000000000000000000FFFFFF007F7F7F00FFFFFF00FFFF
      FF00FFFFFF000000FF000000FF000000FF00BFBFBF0080800000FFFFFF00FFFF
      FF00FFFFFF007F7F7F00FFFFFF00FFFFFF00000000007F7F7F00000000000000
      0000000000000000000000000000808000000000000080800000000000000000
      0000000000007F7F7F0000000000000000000000000000000000FFFF0000FFFF
      0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF
      0000FFFF0000FFFF00008080000000000000000000007F7F7F00FFFF0000FFFF
      0000FFFF0000FFFF0000FFFF0000808000000000000080800000FFFF0000FFFF
      0000FFFF00007F7F7F000000000000000000FFFFFF007F7F7F00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF000000FF000000FF000000FF0080800000FFFFFF00FFFF
      FF00FFFFFF007F7F7F00FFFFFF00FFFFFF00000000007F7F7F00000000000000
      0000000000000000000000000000BFBFBF00BFBFBF0080800000000000000000
      0000000000007F7F7F0000000000000000000000000000000000FFFF0000FFFF
      0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF
      0000FFFF0000FFFF0000FFFF000000000000000000007F7F7F00FFFF0000FFFF
      0000FFFF0000FFFF0000FFFF0000BFBFBF00BFBFBF0080800000FFFF0000FFFF
      0000FFFF00007F7F7F000000000000000000FFFFFF007F7F7F00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF000000FF000000FF000000FF00FFFFFF00FFFF
      FF00FFFFFF007F7F7F00FFFFFF00FFFFFF00000000007F7F7F00000000000000
      0000000000000000000000000000BFBFBF00FFFF0000BFBFBF00000000000000
      0000000000007F7F7F0000000000000000000000000000000000FFFF0000FFFF
      0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF
      0000FFFF0000FFFF00000000000000000000000000007F7F7F00FFFF0000FFFF
      0000FFFF0000FFFF0000FFFF0000BFBFBF00FFFF0000BFBFBF00FFFF0000FFFF
      0000FFFF00007F7F7F000000000000000000FFFFFF007F7F7F00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00BFBFBF000000FF000000FF000000FF00FFFF
      FF00FFFFFF007F7F7F00FFFFFF00FFFFFF00000000007F7F7F00000000000000
      000000000000000000000000000080800000FFFF000000000000000000000000
      0000000000007F7F7F0000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFF0000FFFF0000FFFF
      0000FFFF0000000000000000000000000000000000007F7F7F00FFFF0000FFFF
      0000FFFF0000FFFF0000FFFF000080800000FFFF0000FFFF0000FFFF0000FFFF
      0000FFFF00007F7F7F000000000000000000FFFFFF007F7F7F00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0080800000FFFF00000000FF000000FF000000
      FF00FFFFFF007F7F7F00FFFFFF00FFFFFF00000000007F7F7F00000000000000
      0000000000000000000000000000BFBFBF000000000000000000000000000000
      0000000000007F7F7F0000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFF0000FFFF0000FFFF
      000000000000000000000000000000000000000000007F7F7F00FFFF0000FFFF
      0000FFFF0000FFFF0000FFFF0000BFBFBF00FFFF0000FFFF0000FFFF0000FFFF
      0000FFFF00007F7F7F000000000000000000FFFFFF007F7F7F00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00BFBFBF00FFFFFF00FFFFFF000000FF000000
      FF000000FF007F7F7F00FFFFFF00FFFFFF00000000007F7F7F00000000000000
      00000000000000000000000000000000000000000000000000007F7F7F007F7F
      7F007F7F7F007F7F7F0000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFF0000FFFF00000000
      000000000000000000000000000000000000000000007F7F7F00FFFF0000FFFF
      0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF00007F7F7F007F7F
      7F007F7F7F007F7F7F000000000000000000FFFFFF007F7F7F00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF007F7F7F000000
      FF000000FF000000FF00FFFFFF00FFFFFF00000000007F7F7F00000000000000
      00000000000000000000000000000000000000000000000000007F7F7F000000
      00007F7F7F000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFF0000000000000000
      000000000000000000000000000000000000000000007F7F7F00FFFF0000FFFF
      0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF00007F7F7F000000
      00007F7F7F00000000000000000000000000FFFFFF007F7F7F00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF007F7F7F00FFFF
      FF000000FF000000FF000000FF00FFFFFF00000000007F7F7F00000000000000
      00000000000000000000000000000000000000000000000000007F7F7F007F7F
      7F00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000007F7F7F00FFFF0000FFFF
      0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF00007F7F7F007F7F
      7F0000000000000000000000000000000000FFFFFF007F7F7F00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF007F7F7F007F7F
      7F00FFFFFF000000FF000000FF000000FF00000000007F7F7F007F7F7F007F7F
      7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000007F7F7F007F7F7F007F7F
      7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F000000
      000000000000000000000000000000000000FFFFFF007F7F7F007F7F7F007F7F
      7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F00FFFF
      FF00FFFFFF00FFFFFF000000FF000000FF00424D3E000000000000003E000000
      2800000040000000200000000100010000000000000100000000000000000000
      000000000000000000000000FFFFFF0000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000008003FFFF80030000BFFBFFFF80030000
      B1FBFFBF80030000B0FBFF9F80030000B07BFF8F80030000B03BFF8780030000
      BC3BC00380030000BEBBC00180830000BE3BC00180030000BE3BC00380030000
      BE7BFF8780030000BEFBFF8F80030000BFC3FF9F80030000BFD7FFBF80170000
      BFCFFFFF800F0000801FFFFF801F000000000000000000000000000000000000
      000000000000}
  end
  object Timer1: TTimer
    Enabled = False
    OnTimer = Timer1Timer
    Left = 216
    Top = 616
  end
  object ImageList2: TImageList
    Left = 342
    Top = 550
    Bitmap = {
      494C010103008805880510001000FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
      0000000000003600000028000000400000001000000001002000000000000010
      00000000000000000000000000000000000000000000078DBE00078DBE00078D
      BE00078DBE00078DBE00078DBE00078DBE00078DBE00078DBE00078DBE00078D
      BE00078DBE000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000732DE000732DE000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000078DBE0025A1D10071C6E80084D7
      FA0066CDF90065CDF90065CDF90065CDF90065CDF80065CDF90065CDF80066CE
      F9003AADD8001999C9000000000000000000000000000732DE000732DE000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000732DE000732DE00000000000000000000000000000000000000
      000000000000FFFFFF007F7F7F000000FF007F7F7F00FFFFFF00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000078DBE004CBCE70039A8D100A0E2
      FB006FD4FA006FD4F9006ED4FA006FD4F9006FD4FA006FD4FA006FD4FA006ED4
      F9003EB1D900C9F0F300078DBE0000000000000000000732DE000732DE000732
      DE00000000000000000000000000000000000000000000000000000000000000
      00000732DE000732DE00000000000000000000000000000000000000000000FF
      FF00FFFFFF0000FFFF000000FF000000FF000000FF0000FFFF00FFFFFF0000FF
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000078DBE0072D6FA00078DBE00AEE9
      FC0079DCFB0079DCFB0079DCFB0079DCFB0079DCFB007ADCFB0079DCFA0079DC
      FA0044B5D900C9F0F300078DBE0000000000000000000732DE000732DD000732
      DE000732DE000000000000000000000000000000000000000000000000000732
      DE000732DE00000000000000000000000000000000000000000000FFFF00FFFF
      FF0000FFFF00FFFFFF007F7F7F000000FF007F7F7F00FFFFFF0000FFFF00FFFF
      FF0000FFFF000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000078DBE0079DDFB001899C7009ADF
      F30092E7FC0084E4FB0083E4FC0083E4FC0084E4FC0083E4FC0083E4FB0084E5
      FC0048B9DA00C9F0F3001496C4000000000000000000000000000534ED000732
      DF000732DE000732DE00000000000000000000000000000000000732DE000732
      DE00000000000000000000000000000000000000000000FFFF00FFFFFF0000FF
      FF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FF
      FF00FFFFFF0000FFFF0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000078DBE0082E3FC0043B7DC0065C2
      E000ABF0FC008DEBFC008DEBFC008DEBFD008DEBFD008DEBFC008DEBFD008DEB
      FC004CBBDA00C9F0F300C9F0F300078DBE000000000000000000000000000000
      00000732DE000732DE000732DD00000000000732DD000732DE000732DE000000
      00000000000000000000000000000000000000000000FFFFFF0000FFFF00FFFF
      FF0000FFFF00FFFFFF0000FFFF000000FF0000FFFF00FFFFFF0000FFFF00FFFF
      FF0000FFFF00FFFFFF0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000078DBE008AEAFC0077DCF300219C
      C700FEFFFF00C8F7FD00C9F7FD00C9F7FD00C9F7FE00C8F7FE00C9F7FD00C8F7
      FE009BD5E600EAFEFE00D2F3F800078DBE000000000000000000000000000000
      0000000000000732DD000633E6000633E6000633E9000732DC00000000000000
      000000000000000000000000000000000000FFFFFF0000FFFF00FFFFFF0000FF
      FF00FFFFFF0000FFFF00FFFFFF000000FF007F7F7F0000FFFF00FFFFFF0000FF
      FF00FFFFFF0000FFFF00FFFFFF00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000078DBE0093F0FE0093F0FD001697
      C500078DBE00078DBE00078DBE00078DBE00078DBE00078DBE00078DBE00078D
      BE00078DBE00078DBE00078DBE00078DBE000000000000000000000000000000
      000000000000000000000633E3000732E3000534EF0000000000000000000000
      00000000000000000000000000000000000000FFFF00FFFFFF0000FFFF00FFFF
      FF0000FFFF00FFFFFF0000FFFF000000FF000000FF00FFFFFF0000FFFF00FFFF
      FF0000FFFF00FFFFFF0000FFFF00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000078DBE009BF5FE009AF6FE009AF6
      FE009BF5FD009BF6FE009AF6FE009BF5FE009AF6FD009BF5FE009AF6FE009AF6
      FE000989BA000000000000000000000000000000000000000000000000000000
      0000000000000732DD000534ED000533E9000434EF000434F500000000000000
      000000000000000000000000000000000000FFFFFF0000FFFF00FFFFFF0000FF
      FF00FFFFFF0000FFFF00FFFFFF0000FFFF000000FF000000FF00FFFFFF0000FF
      FF00FFFFFF0000FFFF00FFFFFF00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000078DBE00FEFEFE00A0FBFF00A0FB
      FE00A0FBFE00A1FAFE00A1FBFE00A0FAFE00A1FBFE00A1FBFF00A0FBFF00A1FB
      FF000989BA000000000000000000000000000000000000000000000000000000
      00000434F4000534EF000533EB0000000000000000000434F4000335F8000000
      00000000000000000000000000000000000000FFFF00FFFFFF0000FFFF00FFFF
      FF007F7F7F007F7F7F0000FFFF00FFFFFF007F7F7F000000FF000000FF00FFFF
      FF0000FFFF00FFFFFF0000FFFF00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000078DBE00FEFEFE00A5FE
      FF00A5FEFF00A5FEFF00078DBE00078DBE00078DBE00078DBE00078DBE00078D
      BE00000000000000000000000000000000000000000000000000000000000335
      FC000534EF000434F800000000000000000000000000000000000335FC000335
      FB0000000000000000000000000000000000FFFFFF0000FFFF00FFFFFF0000FF
      FF000000FF000000FF00FFFFFF0000FFFF007F7F7F000000FF000000FF0000FF
      FF00FFFFFF0000FFFF00FFFFFF00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000078DBE00078D
      BE00078DBE00078DBE0000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000000335FB000335
      FB000335FC000000000000000000000000000000000000000000000000000335
      FB000335FB0000000000000000000000000000000000FFFFFF0000FFFF00FFFF
      FF000000FF000000FF007F7F7F00FFFFFF007F7F7F000000FF000000FF00FFFF
      FF0000FFFF00FFFFFF0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000335FB000335FB000335
      FB00000000000000000000000000000000000000000000000000000000000000
      0000000000000335FB0000000000000000000000000000FFFF00FFFFFF0000FF
      FF00FFFFFF000000FF000000FF000000FF000000FF000000FF00FFFFFF0000FF
      FF00FFFFFF0000FFFF0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000335FB000335FB000335FB000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000FFFF00FFFF
      FF0000FFFF00FFFFFF000000FF000000FF000000FF00FFFFFF0000FFFF00FFFF
      FF0000FFFF000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000335FB000335FB00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000000000000000FF
      FF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FF
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000424D3E000000000000003E000000
      2800000040000000100000000100010000000000800000000000000000000000
      000000000000000000000000FFFFFF008007FFFCFFFF000000039FF9F83F0000
      00018FF3E00F0000000187E7C00700000001C3CF800300000000F11F80030000
      0000F83F000100000000FC7F000100000007F83F000100000007F19F00010000
      800FE3CF00010000C3FFC7E780030000FFFF8FFB80030000FFFF1FFFC0070000
      FFFF3FFFE00F0000FFFFFFFFF83F000000000000000000000000000000000000
      000000000000}
  end
  object BalloonHint: TBalloonHint
    Images = ImageList2
    Left = 174
    Top = 614
  end
  object OpenDialog2: TOpenDialog
    Filter = 'WAV|*.WAV'
    Left = 136
    Top = 614
  end
  object OpenDialogProgram: TOpenDialog
    Filter = 'Program|*.exe;*.cmd;*.bat'
    Left = 14
    Top = 610
  end
  object SaveDialogUsrOutput: TSaveDialog
    Filter = 'All|*.*'
    Left = 96
    Top = 614
  end
  object OpenDialogUsrInput: TOpenDialog
    Filter = 'All|*.*'
    Left = 56
    Top = 612
  end
end
