object Form4: TForm4
  Left = 0
  Top = 0
  Caption = 'File Information'
  ClientHeight = 427
  ClientWidth = 665
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnShow = FormShow
  DesignSize = (
    665
    427)
  PixelsPerInch = 96
  TextHeight = 13
  object tvTree: TTreeView
    Left = 0
    Top = -2
    Width = 201
    Height = 428
    Anchors = [akLeft, akTop, akBottom]
    Indent = 19
    ReadOnly = True
    TabOrder = 0
    OnClick = tvTreeClick
    ExplicitHeight = 263
  end
  object tvDesc: TValueListEditor
    Left = 200
    Top = -2
    Width = 464
    Height = 428
    Anchors = [akLeft, akTop, akRight, akBottom]
    DisplayOptions = [doAutoColResize, doKeyColFixed]
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    GridLineWidth = 0
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goColSizing, goThumbTracking]
    ParentFont = False
    ScrollBars = ssVertical
    Strings.Strings = (
      'Offset='
      'Size=')
    TabOrder = 1
    TitleCaptions.Strings = (
      'Name'
      'Value')
    Visible = False
    ExplicitWidth = 309
    ExplicitHeight = 263
    ColWidths = (
      123
      335)
  end
end
